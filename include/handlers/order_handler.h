#ifndef ORDER_HANDLER_H
#define ORDER_HANDLER_H

#include "menu_handler_base.h"

class OrderHandler : public MenuHandlerBase {
public:
    using MenuHandlerBase::MenuHandlerBase;
    
    void handleOrderManagement(const std::string& username);
    void displayOrderDetails(const std::shared_ptr<Order>& order);
    void handleOrderOperation(int choice, const std::shared_ptr<Order>& order,
                            const std::string& username);
    void displayOrderSummary(const std::shared_ptr<Order>& order);
    std::string getOrderStatusString(OrderStatus status);
};

#endif