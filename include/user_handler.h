#ifndef USER_HANDLER_H
#define USER_HANDLER_H

#include "menu_handler_base.h"

class UserHandler : public MenuHandlerBase {
public:
    using MenuHandlerBase::MenuHandlerBase;
    
    std::shared_ptr<User> handleLogin();
    bool handleRegister();
    void handleBalance(const std::string& username);
    void handleChangePassword(const std::string& username);
    void handleAccountInfo(const std::shared_ptr<User>& currentUser);
};

#endif