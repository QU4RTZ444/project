#include "menu.h"
#include "platform_utils.h"
#include "menu_handler.h"

class Menu::MenuImpl {
private:
    static const int ITEMS_PER_PAGE = 5;
    UserManager userManager;
    ProductManager productManager;
    MenuHandler handler;

public:
    MenuImpl() : handler(userManager, productManager) {}

    void displayMainMenu() {
        while(true) {
            Menu::showMainMenu();
            int choice = Menu::getChoice(0, 3);
            
            switch(choice) {
                case 0:
                    std::cout << "感谢使用，再见！\n";
                    return;
                    
                case 1: {
                    auto currentUser = handler.handleLogin();
                    if (currentUser) {
                        displayUserMenu(currentUser);
                    }
                    handler.waitForKey();
                    break;
                }
                    
                case 2: {
                    bool success = handler.handleRegister();
                    if (success) {
                        std::cout << "注册成功！\n";
                    } else {
                        std::cout << "注册失败：用户名可能已存在\n";
                    }
                    handler.waitForKey();
                    break;
                }
                    
                case 3: {
                    handler.handleBrowseProducts();
                    handler.waitForKey();
                    break;
                }
            }
        }
    }

    void displayUserMenu(const std::shared_ptr<User>& currentUser) {
        while (true) {
            Menu::clearScreen();
            if (currentUser->getUserType() == "消费者") {
                Menu::showConsumerMenu();
                if (handleConsumerChoice(currentUser)) {
                    return;  // 如果返回 true，则退出到主菜单
                }
            } else if (currentUser->getUserType() == "商家") {
                Menu::showSellerMenu();
                if (handleSellerChoice(currentUser)) {
                    return;  // 如果返回 true，则退出到主菜单
                }
            }
            handler.waitForKey();
        }
    }

private:
    // 修改返回类型为 bool，true 表示要退出到上层菜单
    bool handleConsumerChoice(const std::shared_ptr<User>& currentUser) {
        int choice = Menu::getChoice(0, 6);
        switch(choice) {
            case 0: 
                return true;  // 返回上层菜单
            case 1: 
                handler.handleBrowseProducts();
                break;
            case 2:
                handler.handleSearchProducts();
                break;
            case 3:
                std::cout << "购物车功能开发中...\n";
                break;
            case 4:
                std::cout << "订单历史功能开发中...\n";
                break;
            case 5:
                handler.handleBalance(currentUser->getUsername());
                break;
            case 6:
                handler.handleChangePassword(currentUser->getUsername());
                break;
        }
        return false;  // 继续当前菜单
    }

    bool handleSellerChoice(const std::shared_ptr<User>& currentUser) {
        int choice = Menu::getChoice(0, 5);
        switch(choice) {
            case 0: 
                return true;  // 返回上层菜单
            case 1:
                handler.handleAddProduct(currentUser->getUsername());
                break;
            case 2:
                handler.handleManageProducts(currentUser->getUsername());
                break;
            case 3:
                std::cout << "销售统计功能开发中...\n";
                break;
            case 4:
                handler.handleBalance(currentUser->getUsername());
                break;
            case 5:
                handler.handleChangePassword(currentUser->getUsername());
                break;
        }
        return false;  // 继续当前菜单
    }
};

// Menu 类的公共成员函数实现
void Menu::displayMainMenu() {
    MenuImpl impl;
    impl.displayMainMenu();
}

void Menu::displayUserMenu(const std::shared_ptr<User>& currentUser) {
    MenuImpl impl;
    impl.displayUserMenu(currentUser);
}

void Menu::showMainMenu() {
    clearScreen();
    std::cout << "\n┌────────────────────────────────┐\n";
    std::cout << "│        电商交易平台主菜单      │\n";
    std::cout << "├────────────────────────────────┤\n";
    std::cout << "│  1. 用户登录                   │\n";
    std::cout << "│  2. 用户注册                   │\n";
    std::cout << "│  3. 浏览所有商品               │\n";
    std::cout << "│  0. 退出系统                   │\n";
    std::cout << "└────────────────────────────────┘\n";
    std::cout <<"输入选择(0~3)\n";
}

void Menu::showConsumerMenu() {
    std::cout << "\n==== 消费者菜单 ====\n";
    std::cout << "1. 查看商品列表\n";
    std::cout << "2. 搜索商品\n";
    std::cout << "3. 查看购物车\n";
    std::cout << "4. 查看订单历史\n";
    std::cout << "5. 账户余额管理\n";
    std::cout << "6. 修改密码\n";
    std::cout << "0. 退出登录\n";
    std::cout << "请选择操作 [0-6]: ";
}

void Menu::showSellerMenu() {
    std::cout << "\n==== 商家菜单 ====\n";
    std::cout << "1. 添加新商品\n";
    std::cout << "2. 管理商品\n";
    std::cout << "3. 查看销售统计\n";
    std::cout << "4. 账户余额查询\n";
    std::cout << "5. 修改密码\n";
    std::cout << "0. 退出登录\n";
    std::cout << "请选择操作 [0-5]: ";
}

int Menu::getChoice(int min, int max) {
    int choice;
    while (true) {
        if (std::cin >> choice && choice >= min && choice <= max) {
            return choice;
        }
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "输入无效，请重新输入 [" << min << "-" << max << "]: ";
    }
}

void Menu::clearScreen() {
    PlatformUtils::clearScreen();
}