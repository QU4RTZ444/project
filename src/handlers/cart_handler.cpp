#include "include.h"

void CartHandler::handleShoppingCart(const std::string& username) {
    ShoppingCart cart(username);

    // 从数据库加载购物车商品
    auto cartItems = db->getCartItems(username);
    for (const auto& [productId, quantity] : cartItems) {
        try {
            auto product = productManager.getProduct(productId);
            if (product) {
                cart.addItem(product, quantity);
            }
        } catch (const std::exception& e) {
            std::cerr << "加载商品失败: " << e.what() << "\n";
        }
    }

    while (true) {
        Menu::clearScreen();
        std::cout << "\n┌────────────────────────────────┐\n";
        std::cout << "│            购物车              │\n";
        std::cout << "├────────────────────────────────┤\n";
        
        // 显示总金额
        double total = cart.calculateTotal();
        std::cout << "总金额: ￥" << std::fixed << std::setprecision(2) << total << "\n\n";

        std::cout << "1. 查看购物车\n";
        std::cout << "2. 修改商品数量\n";
        std::cout << "3. 删除商品\n";
        std::cout << "4. 清空购物车\n";
        std::cout << "5. 结算\n";
        std::cout << "0. 返回\n";
        std::cout << "└────────────────────────────────┘\n";
        std::cout << "请选择操作 [0-5]: ";

        int choice = Menu::getChoice(0, 5);
        if (choice == 0) break;

        try {
            switch (choice) {
                case 1:
                    displayCart(cart);
                    waitForKey();  // 只在这里调用一次
                    break;
                    
                case 2:
                    handleUpdateCartItem(cart);
                    // 更新数据库
                    for (const auto& item : cart.getItems()) {
                        db->saveCartItem(username, item.getProduct()->getId(), 
                                       item.getQuantity());
                    }
                    break;
                    
                case 3:
                    handleRemoveCartItem(cart);
                    break;
                    
                case 4:
                    if (confirmAction("确认清空购物车？")) {
                        try {
                            // 先清空数据库中的购物车记录
                            if (db->removeCartItem(username, -1)) {
                                // 成功清空数据库后再清空内存中的购物车
                                cart.clear();
                                std::cout << "购物车已清空\n";
                            } else {
                                std::cout << "清空购物车失败\n";
                            }
                        } catch (const std::exception& e) {
                            std::cerr << "清空购物车失败: " << e.what() << "\n";
                        }
                    }
                    break;
                    
                case 5:
                    if (!cart.getItems().empty()) {
                        handleCheckout(cart, username);
                    } else {
                        std::cout << "购物车为空，无法结算\n";
                    }
                    break;
            }
            
        } catch (const std::exception& e) {
            std::cerr << "操作失败: " << e.what() << "\n";
            waitForKey();
        }
    }
}

bool CartHandler::displayCart(const ShoppingCart& cart) const {
    const auto& items = cart.getItems();
    if (items.empty()) {
        std::cout << "购物车为空\n";
        return false;
    }

    std::cout << "\n=== 购物车内容 ===\n";
    double total = 0.0;
    for (const auto& item : items) {
        auto product = item.getProduct();
        std::cout << "\n--------------------------------\n";
        product->display();
        std::cout << "数量: " << item.getQuantity() << "\n";
        
        double discountedPrice = product->getPrice();
        double itemTotal = discountedPrice * item.getQuantity();
        
        if (product->getDiscountRate() < 1.0) {
            double originalTotal = product->getBasePrice() * item.getQuantity();
            std::cout << "原价小计: ￥" << std::fixed << std::setprecision(2) 
                     << originalTotal << "\n";
            std::cout << "折后小计: ￥" << std::fixed << std::setprecision(2) 
                     << itemTotal << "\n";
        } else {
            std::cout << "小计: ￥" << std::fixed << std::setprecision(2) 
                     << itemTotal << "\n";
        }
        total += itemTotal;
    }
    
    std::cout << "\n--------------------------------\n";
    std::cout << "总计: ￥" << std::fixed << std::setprecision(2) << total << "\n";
    return true;
}

void CartHandler::handleUpdateCartItem(ShoppingCart& cart) {
    if (!displayCart(cart)) {
        return;
    }

    std::cout << "\n请输入要修改的商品ID: ";
    int productId;
    if (!(std::cin >> productId)) {
        std::cout << "无效的商品ID\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return;
    }

    std::cout << "请输入新的数量: ";
    int newQuantity;
    if (std::cin >> newQuantity && newQuantity > 0) {
        try {
            if (cart.updateQuantity(productId, newQuantity)) {
                std::cout << "数量更新成功\n";
            } else {
                std::cout << "商品不在购物车中\n";
            }
        } catch (std::runtime_error& e) {
            std::cout << "更新失败: " << e.what() << "\n";
        }
    } else {
        std::cout << "无效的数量\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

void CartHandler::handleRemoveCartItem(ShoppingCart& cart) {
    if (!displayCart(cart)) {
        return;
    }

    std::cout << "\n请输入要删除的商品ID: ";
    int productId;
    if (std::cin >> productId) {
        if (cart.removeItem(productId)) {
            // 同步更新数据库
            if (db->removeCartItem(cart.getUsername(), productId)) {
                std::cout << "商品已从购物车移除\n";
            } else {
                std::cout << "商品移除失败\n";
            }
        } else {
            std::cout << "商品不在购物车中\n";
        }
    } else {
        std::cout << "无效的商品ID\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

void CartHandler::handleCheckout(ShoppingCart& cart, const std::string& username) {
    if (!displayCart(cart)) {
        return;
    }

    if (!confirmAction("确认结算购物车？")) {
        return;
    }

    try {
        OrderManager orderManager;
        auto order = orderManager.createOrder(cart);
        if (!order) {
            throw std::runtime_error("创建订单失败");
        }

        std::cout << "订单创建成功！订单号: " << order->getId() << "\n";
        std::cout << "订单总额: ￥" << std::fixed << std::setprecision(2) 
                 << order->getTotalAmount() << "\n";
        
        if (confirmAction("是否立即支付？")) {
            try {
                if (orderManager.payOrder(order->getId(), username)) {
                    std::cout << "支付成功！\n";
                    
                    // 支付成功后清空购物车
                    try {
                        if (db->removeCartItem(username, -1)) {
                            cart.clear();
                            std::cout << "购物车已清空\n";
                        } else {
                            std::cout << "清空购物车失败\n";
                        }
                    } catch (const std::exception& e) {
                        std::cerr << "清空购物车失败: " << e.what() << "\n";
                    }
                } else {
                    throw std::runtime_error("支付处理失败");
                }
            } catch (const std::exception& e) {
                // 如果支付失败，尝试取消订单
                try {
                    if (!orderManager.cancelOrder(order->getId(), username)) {
                        std::cerr << "警告：订单取消失败，请联系客服处理\n";
                    }
                } catch (const std::exception& e) {
                    std::cerr << "警告：订单取消失败，请联系客服处理\n";
                }
            }
        } else {
            std::cout << "您可以稍后在\"我的订单\"中完成支付\n";
        }
    } catch (const std::exception& e) {
        std::cout << "结算失败: " << e.what() << "\n";
    }
    waitForKey();
}

void CartHandler::handleAddToCart(const std::shared_ptr<Product>& product, const std::string& username) {
    if (!product) {
        std::cout << "无效的商品\n";
        return;
    }

    std::cout << "请输入购买数量: ";
    int quantity;
    if (std::cin >> quantity && quantity > 0) {
        try {
            ShoppingCart cart(username);
            // 从数据库加载现有购物车
            auto cartItems = db->getCartItems(username);
            for (const auto& [productId, qty] : cartItems) {
                auto p = productManager.getProduct(productId);
                if (p) {
                    cart.addItem(p, qty);
                }
            }

            // 添加新商品
            if (cart.addItem(product, quantity)) {
                if (db->saveCartItem(username, product->getId(), quantity)) {
                    std::cout << "成功添加到购物车！\n";
                } else {
                    std::cout << "保存到数据库失败\n";
                }
            }
        } catch (const std::exception& e) {
            std::cout << "添加失败: " << e.what() << "\n";
        }
    } else {
        std::cout << "无效的数量\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    waitForKey();
}