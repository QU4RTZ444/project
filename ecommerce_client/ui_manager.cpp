#include "ui_manager.h"
#include "utils.h"
#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>

void UIManager::showSystemTitle() {
    Utils::clearScreen();
    std::cout << "\n";
    std::cout << "    ================================================\n";
    std::cout << "    |                                              |\n";
    std::cout << "    |              电商交易平台系统                  |\n";
    std::cout << "    |           E-Commerce Platform                |\n";
    std::cout << "    |                                              |\n";
    std::cout << "    |                版本 1.0                     |\n";
    std::cout << "    |                                              |\n";
    std::cout << "    ================================================\n";
    std::cout << std::endl;
}

void UIManager::showWelcomeMessage() {
    showSystemTitle();
    Utils::showInfo("欢迎使用电商交易平台！");
    Utils::showInfo("系统正在连接服务器...");
    Sleep(1000); // Windows Sleep函数
}

void UIManager::showMainMenu() {
    Utils::clearScreen();
    showSystemTitle();
    Utils::showSeparator("主菜单");

    std::cout << "    +-------------------------------------+\n";
    std::cout << "    |  1. 用户注册                        |\n";
    std::cout << "    |  2. 用户登录                        |\n";
    std::cout << "    |  3. 浏览商品                        |\n";
    std::cout << "    |  4. 退出系统                        |\n";
    std::cout << "    +-------------------------------------+\n";
    std::cout << std::endl;
}

void UIManager::showProductBrowseMenu() {
    Utils::clearScreen();
    showSystemTitle();
    Utils::showSeparator("商品浏览");

    std::cout << "    +-------------------------------------+\n";
    std::cout << "    |  1. 查看商品列表                    |\n";
    std::cout << "    |  2. 搜索商品                        |\n";
    std::cout << "    |  3. 返回主菜单                      |\n";
    std::cout << "    +-------------------------------------+\n";
    std::cout << std::endl;
}

void UIManager::showProductList() {
    Utils::clearScreen();
    showSystemTitle();
    Utils::showSeparator("商品列表");
}

void UIManager::showProductSearchForm() {
    Utils::clearScreen();
    showSystemTitle();
    Utils::showSeparator("商品搜索");

    std::cout << "请输入搜索关键词：" << std::endl;
    std::cout << std::endl;
}

void UIManager::showProductDetail() {
    Utils::clearScreen();
    showSystemTitle();
    Utils::showSeparator("商品详情");
}

void UIManager::showLoggedInMenu(const std::string& username, const std::string& userType, double balance) {
    Utils::clearScreen();
    showSystemTitle();
    Utils::showSeparator("用户中心");

    std::cout << "    +-------------------------------------+\n";
    std::cout << "    |  用户: " << std::left << std::setw(28) << username << "|\n";
    std::cout << "    |  类型: " << std::left << std::setw(28) << userType << "|\n";
    std::cout << "    |  余额: " << std::left << std::setw(28) << Utils::formatMoney(balance) << "|\n";
    std::cout << "    +-------------------------------------+\n";
    std::cout << std::endl;

    std::cout << "    +-------------------------------------+\n";
    std::cout << "    |  1. 查看商品                        |\n";
    std::cout << "    |  2. 搜索商品                        |\n";

    if (userType == "消费者") {
        std::cout << "    |  3. 购物车管理                      |\n";
    }
    else {
        std::cout << "    |  3. 商品管理                        |\n";
    }

    std::cout << "    |  4. 订单管理                        |\n";  // 商家和消费者都有订单管理
    std::cout << "    |  5. 账户管理                        |\n";
    std::cout << "    |  6. 用户登出                        |\n";
    std::cout << "    +-------------------------------------+\n";
    std::cout << std::endl;
}

void UIManager::showRegisterForm() {
    Utils::clearScreen();
    showSystemTitle();
    Utils::showSeparator("用户注册");

    std::cout << "请填写注册信息：" << std::endl;
    std::cout << std::endl;
}

void UIManager::showLoginForm() {
    Utils::clearScreen();
    showSystemTitle();
    Utils::showSeparator("用户登录");

    std::cout << "请输入登录信息：" << std::endl;
    std::cout << std::endl;
}

void UIManager::showMerchantProductMenu() {
    Utils::clearScreen();
    showSystemTitle();
    Utils::showSeparator("商品管理");

    std::cout << "    +-------------------------------------+\n";
    std::cout << "    |  1. 添加商品                        |\n";
    std::cout << "    |  2. 查看我的商品                    |\n";
    std::cout << "    |  3. 修改商品信息                    |\n";
    std::cout << "    |  4. 设置商品折扣                    |\n";
    std::cout << "    |  5. 返回主菜单                      |\n";
    std::cout << "    +-------------------------------------+\n";
    std::cout << std::endl;
}

void UIManager::showAddProductForm() {
    Utils::clearScreen();
    showSystemTitle();
    Utils::showSeparator("添加商品");

    std::cout << "请填写商品信息：" << std::endl;
    std::cout << "支持的商品类型：" << std::endl;
    std::cout << "  1. 食品" << std::endl;
    std::cout << "  2. 书籍" << std::endl;
    std::cout << "  3. 衣服" << std::endl;
    std::cout << std::endl;
}

void UIManager::showModifyProductForm() {
    Utils::clearScreen();
    showSystemTitle();
    Utils::showSeparator("修改商品信息");

    std::cout << "请从下方商品列表中选择要修改的商品：" << std::endl;
    std::cout << "提示：不需要修改的字段可以直接按回车跳过" << std::endl;
    std::cout << std::endl;
}

void UIManager::showMerchantProductList() {
    Utils::clearScreen();
    showSystemTitle();
    Utils::showSeparator("我的商品");
}

void UIManager::showSetDiscountForm() {
    Utils::clearScreen();
    showSystemTitle();
    Utils::showSeparator("设置商品折扣");

    std::cout << "请选择折扣设置方式：" << std::endl;
    std::cout << "折扣范围：0.1 - 1.0 (例如0.8表示8折)" << std::endl;
    std::cout << std::endl;
}

void UIManager::showConsumerMenu() {
    Utils::clearScreen();
    showSystemTitle();
    Utils::showSeparator("消费者功能菜单");

    std::cout << "    +-------------------------------------+\n";
    std::cout << "    |  1. 商品浏览                        |\n";
    std::cout << "    |  2. 商品搜索                        |\n";
    std::cout << "    |  3. 购物车管理                      |\n";
    std::cout << "    |  4. 订单管理                        |\n";  // 添加订单管理
    std::cout << "    |  5. 修改密码                        |\n";
    std::cout << "    |  6. 退出登录                        |\n";
    std::cout << "    +-------------------------------------+\n";
    std::cout << "\n    请选择 (1-6): ";
}

void UIManager::showCartMenu() {
    Utils::clearScreen();
    showSystemTitle();
    Utils::showSeparator("购物车管理");

    std::cout << "    +-------------------------------------+\n";
    std::cout << "    |  1. 查看购物车                      |\n";
    std::cout << "    |  2. 修改商品数量                    |\n";
    std::cout << "    |  3. 移除商品                        |\n";
    std::cout << "    |  4. 清空购物车                      |\n";
    std::cout << "    |  5. 结算购物车                      |\n";
    std::cout << "    |  6. 返回上级菜单                    |\n";
    std::cout << "    +-------------------------------------+\n";
    std::cout << "\n    请选择 (1-6): ";
}

void UIManager::showCheckoutInterface() {
    Utils::clearScreen();
    showSystemTitle();
    Utils::showSeparator("购物车结算");

    std::cout << "正在准备结算信息..." << std::endl;
}

void UIManager::showCheckoutConfirmation() {
    Utils::clearScreen();
    showSystemTitle();
    Utils::showSeparator("确认订单");

    std::cout << "请确认以下订单信息：" << std::endl;
    std::cout << "============================================================" << std::endl;
}

void UIManager::showOrderMenu() {
    Utils::clearScreen();
    showSystemTitle();
    Utils::showSeparator("订单管理");

    std::cout << "    +-------------------------------------+\n";
    std::cout << "    |  1. 查看订单列表                    |\n";
    std::cout << "    |  2. 查看订单详情                    |\n";
    std::cout << "    |  3. 返回上级菜单                    |\n";
    std::cout << "    +-------------------------------------+\n";
    std::cout << "\n    请选择 (1-3): ";
}

void UIManager::showOrderList() {
    Utils::clearScreen();
    showSystemTitle();
    Utils::showSeparator("订单列表");

    std::cout << std::endl;
}

void UIManager::showOrderDetail() {
    Utils::clearScreen();
    showSystemTitle();
    Utils::showSeparator("订单详情");

    std::cout << "订单详细信息：" << std::endl;
    std::cout << std::endl;
}

void UIManager::showAccountMenu() {
    Utils::clearScreen();
    showSystemTitle();
    Utils::showSeparator("账户管理");

    std::cout << "    +-------------------------------------+\n";
    std::cout << "    |  1. 修改密码                        |\n";
    std::cout << "    |  2. 查看账户信息                    |\n";
    std::cout << "    |  3. 账户充值 (开发中)               |\n";
    std::cout << "    |  4. 账户提现 (开发中)               |\n";
    std::cout << "    |  5. 返回主菜单                      |\n";
    std::cout << "    +-------------------------------------+\n";
    std::cout << std::endl;
}

void UIManager::showChangePasswordForm() {
    Utils::clearScreen();
    showSystemTitle();
    Utils::showSeparator("修改密码");

    std::cout << "请输入密码信息：" << std::endl;
    std::cout << std::endl;
}

std::string UIManager::getMenuChoice(int maxChoice) {
    std::string choice;
    while (true) {
        std::cout << "请选择操作 (1-" << maxChoice << "): ";
        std::cin >> choice;

        if (!choice.empty() && choice.length() == 1 &&
            choice[0] >= '1' && choice[0] <= ('0' + maxChoice)) {
            return choice;
        }

        Utils::showError("无效选择，请重新输入！");
    }
}

void UIManager::showLoading(const std::string& message) {
    std::cout << message;
    for (int i = 0; i < 3; ++i) {
        std::cout << ".";
        std::cout.flush();
        Sleep(300); // Windows Sleep函数
    }
    std::cout << std::endl;
}

void UIManager::showExitMessage() {
    Utils::clearScreen();
    showSystemTitle();
    std::cout << "\n谢谢使用电商交易平台！\n" << std::endl;
    std::cout << "再见！\n" << std::endl;
}

void UIManager::showCartDetail() {
    Utils::clearScreen();
    showSystemTitle();
    Utils::showSeparator("购物车详情");

    std::cout << "您的购物车内容如下：" << std::endl;
    std::cout << std::endl;
}

void UIManager::showUpdateCartItemForm() {
    Utils::clearScreen();
    showSystemTitle();
    Utils::showSeparator("修改商品数量");

    std::cout << "请选择要修改数量的商品：" << std::endl;
    std::cout << "提示：输入0可以删除该商品" << std::endl;
    std::cout << std::endl;
}

void UIManager::showRemoveCartItemForm() {
    Utils::clearScreen();
    showSystemTitle();
    Utils::showSeparator("移除购物车商品");

    std::cout << "请选择要移除的商品：" << std::endl;
    std::cout << "提示：此操作将完全移除该商品" << std::endl;
    std::cout << std::endl;
}