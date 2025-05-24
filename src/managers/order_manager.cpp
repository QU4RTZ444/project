#include "include.h"

std::shared_ptr<Order> OrderManager::createOrder(const ShoppingCart& cart) {
    try {
        // 开始事务前检查是否有活动事务
        db->executeQuery("SELECT name FROM sqlite_master LIMIT 1;");  // 测试数据库连接
        db->beginTransaction();
        
        // 计算订单总额
        double totalAmount = cart.calculateTotal();
        const auto& items = cart.getItems();
        
        // 创建订单记录
        int orderId = db->createOrder(cart.getUsername(), totalAmount);
        if (orderId == -1) {
            db->rollback();
            throw std::runtime_error("创建订单失败");
        }
        
        // 为订单中的每个商品创建订单项并锁定库存
        for (const auto& item : items) {
            auto product = item.getProduct();
            if (!db->addOrderItem(orderId, 
                                product->getId(), 
                                item.getQuantity(),
                                product->getPrice(),
                                product->getSeller())) {
                db->rollback();
                throw std::runtime_error("添加订单项失败");
            }
            
            if (!db->lockProductStock(product->getId(), orderId, item.getQuantity())) {
                db->rollback();
                throw std::runtime_error("锁定商品库存失败");
            }
        }
        
        db->commit();
        
        // 创建 Order 对象
        return std::make_shared<Order>(orderId, cart.getUsername(), items);
        
    } catch (const std::exception& e) {
        try {
            db->rollback();
        } catch (...) {
            // 忽略回滚失败的错误
        }
        throw;
    }
}

bool OrderManager::payOrder(int orderId, const std::string& username) {
    try {
        db->beginTransaction();
        
        // 获取订单信息
        auto order = getOrder(orderId);
        if (!order || order->getBuyerUsername() != username) {
            db->rollback();
            throw std::runtime_error("订单不存在或不属于当前用户");
        }
        
        if (order->getStatus() != OrderStatus::PENDING) {
            db->rollback();
            throw std::runtime_error("订单状态不正确");
        }
        
        // 检查用户余额
        double balance = db->getUserBalance(username);
        if (balance < order->getTotalAmount()) {
            db->rollback();
            throw std::runtime_error("余额不足");
        }
        
        // 扣除买家余额
        if (!db->updateBalance(username, balance - order->getTotalAmount())) {
            db->rollback();
            throw std::runtime_error("扣除买家余额失败");
        }

        // 为每个商品更新库存和处理卖家余额
        for (const auto& item : order->getItems()) {
            auto product = item.getProduct();
            
            // 更新商品库存
            const char* updateStockQuery = "UPDATE products SET quantity = quantity - ? "
                                         "WHERE id = ? AND quantity >= ?";
            sqlite3_stmt* stmt;
            if (sqlite3_prepare_v2(db->getHandle(), updateStockQuery, -1, &stmt, nullptr) != SQLITE_OK) {
                db->rollback();
                throw std::runtime_error("准备更新库存语句失败");
            }
            
            sqlite3_bind_int(stmt, 1, item.getQuantity());
            sqlite3_bind_int(stmt, 2, product->getId());
            sqlite3_bind_int(stmt, 3, item.getQuantity());
            
            bool stockUpdated = sqlite3_step(stmt) == SQLITE_DONE;
            sqlite3_finalize(stmt);
            
            if (!stockUpdated) {
                db->rollback();
                throw std::runtime_error("更新商品库存失败");
            }
            
            // 给卖家增加余额
            double itemTotal = product->getPrice() * item.getQuantity();
            double sellerBalance = db->getUserBalance(product->getSeller());
            if (!db->updateBalance(product->getSeller(), sellerBalance + itemTotal)) {
                db->rollback();
                throw std::runtime_error("更新卖家余额失败");
            }

            // 解锁库存
            if (!db->unlockProductStock(product->getId(), orderId)) {
                db->rollback();
                throw std::runtime_error("解锁商品库存失败");
            }
        }
        
        // 更新订单状态
        if (!db->updateOrderStatus(orderId, "paid")) {
            db->rollback();
            throw std::runtime_error("更新订单状态失败");
        }
        
        db->commit();
        return true;
        
    } catch (const std::exception& e) {
        db->rollback();
        throw;
    }
}

bool OrderManager::cancelOrder(int orderId, const std::string& username) {
    try {
        db->beginTransaction();
        
        auto order = getOrder(orderId);
        if (!order || order->getBuyerUsername() != username) {
            db->rollback();
            throw std::runtime_error("订单不存在或不属于当前用户");
        }
        
        if (order->getStatus() != OrderStatus::PENDING) {
            db->rollback();
            throw std::runtime_error("只能取消待支付的订单");
        }
        
        // 解锁商品库存
        for (const auto& item : order->getItems()) {
            if (!db->unlockProductStock(item.getProduct()->getId(), orderId)) {
                db->rollback();
                throw std::runtime_error("解锁商品库存失败");
            }
        }
        
        if (!db->updateOrderStatus(orderId, "cancelled")) {
            db->rollback();
            throw std::runtime_error("更新订单状态失败");
        }
        
        db->commit();
        return true;
        
    } catch (const std::exception& e) {
        db->rollback();
        throw;
    }
}

std::vector<std::shared_ptr<Order>> OrderManager::getUserOrders(const std::string& username) const {
    std::vector<std::shared_ptr<Order>> orders;
    const char* query = "SELECT id, total_amount, status, create_time FROM orders "
                       "WHERE buyer_username = ? ORDER BY create_time DESC;";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db->getHandle(), query, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("准备查询订单语句失败");
    }
    
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int orderId = sqlite3_column_int(stmt, 0);
        double totalAmount = sqlite3_column_double(stmt, 1);
        std::string status = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        std::string createTime = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        
        // 获取订单项
        auto cartItems = getOrderItems(orderId);
        auto order = std::make_shared<Order>(orderId, username, cartItems);
        order->setTotalAmount(totalAmount);
        order->setStatus(status);
        order->setCreateTime(createTime);
        
        orders.push_back(order);
    }
    
    sqlite3_finalize(stmt);
    return orders;
}

std::shared_ptr<Order> OrderManager::getOrder(int orderId) const {
    const char* query = "SELECT buyer_username, total_amount, status, create_time "
                       "FROM orders WHERE id = ?;";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db->getHandle(), query, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("准备查询订单语句失败");
    }
    
    sqlite3_bind_int(stmt, 1, orderId);
    
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        std::string username = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        double totalAmount = sqlite3_column_double(stmt, 1);
        std::string status = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        std::string createTime = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        
        auto cartItems = getOrderItems(orderId);
        auto order = std::make_shared<Order>(orderId, username, cartItems);
        order->setTotalAmount(totalAmount);
        order->setStatus(status);
        order->setCreateTime(createTime);
        
        sqlite3_finalize(stmt);
        return order;
    }
    
    sqlite3_finalize(stmt);
    return nullptr;
}

// 辅助函数：获取订单项
std::vector<CartItem> OrderManager::getOrderItems(int orderId) const {
    std::vector<CartItem> items;
    const char* query = "SELECT oi.product_id, oi.quantity, oi.price, p.name, p.description, "
                       "p.category, p.seller_username "
                       "FROM order_items oi "
                       "JOIN products p ON oi.product_id = p.id "
                       "WHERE oi.order_id = ?;";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db->getHandle(), query, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("准备查询订单项语句失败");
    }
    
    sqlite3_bind_int(stmt, 1, orderId);
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int productId = sqlite3_column_int(stmt, 0);
        int quantity = sqlite3_column_int(stmt, 1);
        double price = sqlite3_column_double(stmt, 2);
        std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        std::string description = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        std::string category = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        std::string seller = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
        
        auto product = createProductFromCategory(productId, name, description, 
                                               price, quantity, seller, category);
        items.emplace_back(product, quantity);
    }
    
    sqlite3_finalize(stmt);
    return items;
}

std::shared_ptr<Product> OrderManager::createProductFromCategory(
    int id, const std::string& name, const std::string& description,
    double price, int quantity, const std::string& seller, 
    const std::string& category) const {
    
    if (category == "图书") {
        return std::make_shared<Book>(id, name, description, price, quantity, seller);
    } 
    else if (category == "食品") {
        return std::make_shared<Food>(id, name, description, price, quantity, seller);
    } 
    else if (category == "服装") {
        return std::make_shared<Clothing>(id, name, description, price, quantity, seller);
    } 
    else {
        throw std::runtime_error("未知的商品类别：" + category);
    }
}