#include "include.h"

std::shared_ptr<User> UserHandler::handleLogin() {
    std::string username, password;
    std::cout << "\n=== 用户登录 ===\n";
    std::cout << "请输入用户名: ";
    std::cin >> username;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    
    std::cout << "请输入密码: ";
    password = PlatformUtils::getHiddenInput();
    
    auto currentUser = userManager.login(username, password);
    if (currentUser) {
        std::cout << "登录成功！欢迎回来，" << username << "！\n";
    } else {
        std::cout << "登录失败：用户名或密码错误\n";
    }
    return currentUser;
}

bool UserHandler::handleRegister() {
    std::string username, password, confirm_password, type;
    std::cout << "\n=== 用户注册 ===\n";
    std::cout << "请输入用户名: ";
    std::cin >> username;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    
    std::cout << "请输入密码: ";
    password = PlatformUtils::getHiddenInput();
    
    std::cout << "请确认密码: ";
    confirm_password = PlatformUtils::getHiddenInput();
    
    if (password != confirm_password) {
        std::cout << "两次输入的密码不一致！\n";
        return false;
    }
    
    std::cout << "请选择用户类型 (1.消费者 2.商家): ";
    int typeChoice = Menu::getChoice(1, 2);
    type = (typeChoice == 1) ? "消费者" : "商家";
    
    return userManager.registerUser(username, password, type);
}

void UserHandler::handleBalance(const std::string& username) {
    double balance = userManager.getBalance(username);
    std::cout << "\n当前余额: " << balance << " 元\n";
    std::cout << "1. 充值\n";
    std::cout << "2. 返回\n";
    
    if (Menu::getChoice(1, 2) == 1) {
        std::cout << "请输入充值金额: ";
        double amount;
        std::cin >> amount;
        if (amount > 0 && userManager.updateBalance(username, balance + amount)) {
            std::cout << "充值成功！\n";
        } else {
            std::cout << "充值失败！\n";
        }
    }
}

void UserHandler::handleChangePassword(const std::string& username) {
    std::cout << "\n=== 修改密码 ===\n";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    
    std::cout << "请输入当前密码: ";
    std::string oldPassword = PlatformUtils::getHiddenInput();
    
    std::cout << "请输入新密码: ";
    std::string newPassword = PlatformUtils::getHiddenInput();
    
    std::cout << "请确认新密码: ";
    std::string confirmPassword = PlatformUtils::getHiddenInput();
    
    if (newPassword != confirmPassword) {
        std::cout << "两次输入的新密码不一致！\n";
        return;
    }
    
    if (userManager.changePassword(username, oldPassword, newPassword)) {
        std::cout << "密码修改成功！\n";
    } else {
        std::cout << "密码修改失败：当前密码可能输入错误\n";
    }
}

void UserHandler::handleAccountInfo(const std::shared_ptr<User>& currentUser) {
    while (true) {
        Menu::clearScreen();
        std::string username = currentUser->getUsername();
        double balance = userManager.getBalance(username);
        
        std::cout << "\n=== 账户信息 ===\n";
        std::cout << "用户名: " << username << "\n";
        std::cout << "用户类型: " << currentUser->getUserType() << "\n";
        std::cout << "账户余额: ￥" << std::fixed << std::setprecision(2) << balance << "\n";

        std::cout << "\n请选择操作：\n";
        std::cout << "1. 充值\n";
        std::cout << "2. 修改密码\n";
        std::cout << "0. 返回\n";

        int choice = Menu::getChoice(0, 2);
        if (choice == 0) break;

        switch (choice) {
            case 1:
                handleBalance(username);  // 复用现有的余额管理功能
                break;
            case 2:
                handleChangePassword(username);  // 复用现有的密码修改功能
                break;
        }
        waitForKey();
    }
}