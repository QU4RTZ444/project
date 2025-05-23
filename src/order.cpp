#include "include.h"

Order::Order(int orderId, const std::string& username, const std::vector<CartItem>& cartItems)
    : id(orderId), 
      buyerUsername(username), 
      items(cartItems),
      status(OrderStatus::PENDING) {
    
    // 计算订单总金额，使用实际折扣价
    totalAmount = 0.0;
    for (const auto& item : items) {
        auto product = item.getProduct();
        // 使用 getPrice() 而不是直接访问 price，因为 getPrice() 会计算折扣
        totalAmount += product->getPrice() * item.getQuantity();
    }
    
    // 设置创建时间
    auto now = std::chrono::system_clock::now();
    auto now_time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&now_time), "%Y-%m-%d %H:%M:%S");
    createTime = ss.str();
}

bool Order::pay() {
    if (status != OrderStatus::PENDING) {
        throw std::runtime_error("订单状态不正确，无法支付");
    }

    try {
        // 获取数据库实例
        auto db = DatabaseManager::getInstance();
        db->beginTransaction();

        // 检查用户余额
        double userBalance = db->getUserBalance(buyerUsername);
        if (userBalance < totalAmount) {
            db->rollback();
            throw std::runtime_error("余额不足");
        }

        // 扣除买家余额
        if (!db->updateBalance(buyerUsername, userBalance - totalAmount)) {
            db->rollback();
            throw std::runtime_error("扣除买家余额失败");
        }

        // 给每个卖家增加余额
        for (const auto& item : items) {
            auto product = item.getProduct();
            std::string seller = product->getSeller();
            double itemTotal = product->getPrice() * item.getQuantity();
            
            // 获取卖家余额
            double sellerBalance = db->getUserBalance(seller);
            
            // 更新卖家余额
            if (!db->updateBalance(seller, sellerBalance + itemTotal)) {
                db->rollback();
                throw std::runtime_error("更新卖家余额失败");
            }

            // 更新商品库存
            if (!db->unlockProductStock(product->getId(), id)) {
                db->rollback();
                throw std::runtime_error("更新商品库存失败");
            }
        }

        // 更新订单状态
        if (!db->updateOrderStatus(id, "paid")) {
            db->rollback();
            throw std::runtime_error("更新订单状态失败");
        }

        status = OrderStatus::PAID;
        db->commit();
        return true;

    } catch (const std::exception& e) {
        try {
            DatabaseManager::getInstance()->rollback();
        } catch (...) {
            // 忽略回滚失败的错误
        }
        throw;
    }
}

bool Order::cancel() {
    if (status != OrderStatus::PENDING) {
        throw std::runtime_error("订单状态不正确，无法取消");
    }

    try {
        // 获取数据库实例
        auto db = DatabaseManager::getInstance();
        db->beginTransaction();

        // 解锁所有商品的库存
        for (const auto& item : items) {
            if (!db->unlockProductStock(item.getProduct()->getId(), id)) {
                db->rollback();
                throw std::runtime_error("解锁商品库存失败");
            }
        }

        // 更新订单状态
        if (!db->updateOrderStatus(id, "cancelled")) {
            db->rollback();
            throw std::runtime_error("更新订单状态失败");
        }

        status = OrderStatus::CANCELLED;
        db->commit();
        return true;

    } catch (const std::exception& e) {
        try {
            DatabaseManager::getInstance()->rollback();
        } catch (...) {
            // 忽略回滚失败的错误
        }
        throw;
    }
}