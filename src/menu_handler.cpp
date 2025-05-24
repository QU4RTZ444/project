#include "include.h"
#include "menu.h"
#include "menu_handler.h"
#include "platform_utils.h"

void MenuHandler::displayUserMenu(const std::shared_ptr<User>& currentUser) {
    while (true) {
        Menu::clearScreen();
        if (currentUser->getUserType() == "消费者") {
            Menu::showConsumerMenu();
            if (handleConsumerChoice(currentUser)) {
                return;
            }
        } else if (currentUser->getUserType() == "商家") {
            Menu::showSellerMenu();
            if (handleSellerChoice(currentUser)) {
                return;
            }
        }
        waitForKey();
    }
}

bool MenuHandler::handleConsumerChoice(const std::shared_ptr<User>& currentUser) {
    while (true) {
        Menu::clearScreen();
        std::cout << "\n=== 消费者菜单 ===\n";
        std::cout << "1. 浏览商品\n";
        std::cout << "2. 搜索商品\n";
        std::cout << "3. 购物车\n";
        std::cout << "4. 我的订单\n";
        std::cout << "5. 账户信息\n";
        std::cout << "0. 退出登录\n";

        int choice = Menu::getChoice(0, 5);
        
        try {
            switch (choice) {
                case 0: return true;
                case 1:
                    getProductHandler().handleBrowseProducts(currentUser);
                    break;
                case 2:
                    getProductHandler().handleSearchProducts(currentUser);
                    break;
                case 3:
                    getCartHandler().handleShoppingCart(currentUser->getUsername());
                    break;
                case 4:
                    getOrderHandler().handleOrderManagement(currentUser->getUsername());
                    break;
                case 5:
                    getUserHandler().handleAccountInfo(currentUser);
                    break;
            }
        } catch (const std::exception& e) {
            std::cerr << "操作失败: " << e.what() << "\n";
            waitForKey();
        }
    }
}

bool MenuHandler::handleSellerChoice(const std::shared_ptr<User>& currentUser) {
    Menu::clearScreen();
    std::cout << "\n=== 商家菜单 ===\n";
    std::cout << "1. 添加商品\n";
    std::cout << "2. 管理商品\n";
    std::cout << "3. 账户余额\n";
    std::cout << "4. 修改密码\n";
    std::cout << "0. 退出登录\n";

    int choice = Menu::getChoice(0, 4);
    
    try {
        switch(choice) {
            case 0: return true;
            case 1:
                getProductHandler().handleAddProduct(currentUser->getUsername());
                break;
            case 2:
                getProductHandler().handleManageProducts(currentUser->getUsername());
                break;
            case 3:
                getUserHandler().handleBalance(currentUser->getUsername());
                break;
            case 4:
                getUserHandler().handleChangePassword(currentUser->getUsername());
                break;
        }
    } catch (const std::exception& e) {
        std::cerr << "操作失败: " << e.what() << "\n";
        waitForKey();
    }
    return false;
}







