#include "include.h"

void OrderHandler::handleOrderManagement(const std::string& username) {
    OrderManager orderManager;
    
    while (true) {
        Menu::clearScreen();
        std::cout << "\n=== 我的订单 ===\n";
        
        auto orders = orderManager.getUserOrders(username);
        if (orders.empty()) {
            std::cout << "暂无订单\n";
            waitForKey();
            return;
        }

        // 添加分页
        int totalPages = (orders.size() + ITEMS_PER_PAGE - 1) / ITEMS_PER_PAGE;
        static int currentPage = 1;

        std::cout << "\n订单列表 (第 " << currentPage << "/" << totalPages << " 页)：\n";
        
        // 计算当前页的订单范围
        int startIdx = (currentPage - 1) * ITEMS_PER_PAGE;
        int endIdx = std::min(startIdx + ITEMS_PER_PAGE, static_cast<int>(orders.size()));

        // 显示当前页的订单
        for (int i = startIdx; i < endIdx; ++i) {
            const auto& order = orders[i];
            std::cout << "\n================================\n";
            std::cout << "订单号: " << order->getId() << "\n";
            std::cout << "创建时间: " << order->getCreateTime() << "\n";
            std::cout << "订单状态: ";
            switch (order->getStatus()) {
                case OrderStatus::PENDING: std::cout << "待支付\n"; break;
                case OrderStatus::PAID: std::cout << "已支付\n"; break;
                case OrderStatus::CANCELLED: std::cout << "已取消\n"; break;
                case OrderStatus::FAILED: std::cout << "支付失败\n"; break;
            }
            std::cout << "订单金额: ￥" << std::fixed << std::setprecision(2) 
                     << order->getTotalAmount() << "\n";
            std::cout << "================================\n";
        }

        std::cout << "\n请选择操作：\n";
        std::cout << "1. 查看订单详情\n";
        std::cout << "2. 支付订单\n";
        std::cout << "3. 取消订单\n";
        std::cout << "4. 下一页\n";
        std::cout << "5. 上一页\n";
        std::cout << "6. 跳转页面\n";
        std::cout << "0. 返回\n";

        int choice = Menu::getChoice(0, 6);
        if (choice == 0) break;

        try {
            switch (choice) {
                case 4: // 下一页
                    if (currentPage < totalPages) {
                        currentPage++;
                        continue;
                    } else {
                        std::cout << "已经是最后一页了\n";
                    }
                    break;
                case 5: // 上一页
                    if (currentPage > 1) {
                        currentPage--;
                        continue;
                    } else {
                        std::cout << "已经是第一页了\n";
                    }
                    break;
                case 6: // 跳转页面
                    std::cout << "请输入页码 (1-" << totalPages << "): ";
                    int pageNum;
                    if (std::cin >> pageNum && pageNum >= 1 && pageNum <= totalPages) {
                        currentPage = pageNum;
                        continue;
                    } else {
                        std::cout << "无效的页码\n";
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    }
                    break;
                default: {
                    std::cout << "请输入订单号: ";
                    int orderId;
                    if (!(std::cin >> orderId)) {
                        std::cout << "无效的订单号\n";
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        continue;
                    }

                    auto order = orderManager.getOrder(orderId);
                    if (!order || order->getBuyerUsername() != username) {
                        std::cout << "订单不存在\n";
                        continue;
                    }

                    switch (choice) {
                        case 1:
                            displayOrderDetails(order);
                            break;
                        case 2:
                            if (order->getStatus() == OrderStatus::PENDING) {
                                if (orderManager.payOrder(orderId, username)) {
                                    std::cout << "支付成功！\n";
                                }
                            } else {
                                std::cout << "该订单不可支付\n";
                            }
                            break;
                        case 3:
                            if (order->getStatus() == OrderStatus::PENDING) {
                                if (orderManager.cancelOrder(orderId, username)) {
                                    std::cout << "订单已取消\n";
                                }
                            } else {
                                std::cout << "该订单不可取消\n";
                            }
                            break;
                    }
                }
            }
        } catch (const std::exception& e) {
            std::cerr << "操作失败: " << e.what() << "\n";
        }
        waitForKey();
    }
}

void OrderHandler::displayOrderDetails(const std::shared_ptr<Order>& order) {
    Menu::clearScreen();
    std::cout << "\n=== 订单详情 ===\n";
    std::cout << "订单号: " << order->getId() << "\n";
    std::cout << "创建时间: " << order->getCreateTime() << "\n";
    std::cout << "订单状态: ";
    switch (order->getStatus()) {
        case OrderStatus::PENDING: std::cout << "待支付\n"; break;
        case OrderStatus::PAID: std::cout << "已支付\n"; break;
        case OrderStatus::CANCELLED: std::cout << "已取消\n"; break;
        case OrderStatus::FAILED: std::cout << "支付失败\n"; break;
    }
    
    std::cout << "\n商品列表：\n";
    for (const auto& item : order->getItems()) {
        std::cout << "\n--------------------------------\n";
        item.getProduct()->display();
        std::cout << "数量: " << item.getQuantity() << "\n";
        double itemTotal = item.getProduct()->getPrice() * item.getQuantity();
        std::cout << "小计: ￥" << std::fixed << std::setprecision(2) << itemTotal << "\n";
    }
    
    std::cout << "\n================================\n";
    std::cout << "订单总额: ￥" << std::fixed << std::setprecision(2) 
              << order->getTotalAmount() << "\n";
    std::cout << "================================\n";
}