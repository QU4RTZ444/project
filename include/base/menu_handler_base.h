#ifndef MENU_HANDLER_BASE_H
#define MENU_HANDLER_BASE_H

#include "include.h"
#include "user_manager.h"
#include "product_manager.h"

class MenuHandlerBase {
protected:
    UserManager& userManager;
    ProductManager& productManager;
    DatabaseManager* db;
    
    /**
     * @brief 确认操作
     * @param message 确认消息
     * @return bool 用户是否确认
     */
    bool confirmAction(const std::string& message) const;

public:
    MenuHandlerBase(UserManager& um, ProductManager& pm) 
        : userManager(um), productManager(pm), db(DatabaseManager::getInstance()) {}
    virtual ~MenuHandlerBase() = default;

    
    /**
     * @brief 等待用户按键继续
     */
    void waitForKey() const;
};

#endif