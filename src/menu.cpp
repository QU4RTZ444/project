#include "menu.h"
#include "platform_utils.h"
#include "menu_handler.h"

class Menu::MenuImpl {
private:
    static const int ITEMS_PER_PAGE = 3;
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
                    try {
                        // 未登录用户只能浏览，不能购买
                        std::shared_ptr<User> anonymousUser = nullptr;
                        handler.handleBrowseProducts(anonymousUser);
                    } catch (const std::exception& e) {
                        std::cerr << "浏览失败: " << e.what() << "\n";
                    }
                    handler.waitForKey();
                    break;
                }
            }
        }
    }

    void displayUserMenu(const std::shared_ptr<User>& currentUser) {
        while (true) {
            try {
                Menu::clearScreen();
                if (currentUser->getUserType() == "消费者") {
                    Menu::showConsumerMenu();
                    if (handler.handleConsumerChoice(currentUser)) {
                        return;
                    }
                } else if (currentUser->getUserType() == "商家") {
                    Menu::showSellerMenu();
                    if (handler.handleSellerChoice(currentUser)) {
                        return;
                    }
                }
            } catch (const std::exception& e) {
                std::cerr << "操作失败: " << e.what() << "\n";
                handler.waitForKey();
            }
        }
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
    std::cout << "3. 账户余额管理\n";
    std::cout << "4. 修改密码\n";
    std::cout << "0. 退出登录\n";
    std::cout << "请选择操作 [0-4]: ";
}

void Menu::showSellerMenu() {
    std::cout << "\n==== 商家菜单 ====\n";
    std::cout << "1. 添加新商品\n";
    std::cout << "2. 管理商品\n";
    std::cout << "3. 账户余额管理\n";
    std::cout << "4. 修改密码\n";
    std::cout << "0. 退出登录\n";
    std::cout << "请选择操作 [0-4]: ";
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