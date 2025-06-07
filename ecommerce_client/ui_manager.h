#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include <string>
#include <windows.h>

class UIManager {
public:
    // 显示系统标题
    static void showSystemTitle();

    // 显示欢迎信息
    static void showWelcomeMessage();

    // 显示主菜单
    static void showMainMenu();

    // 显示登录后菜单
    static void showLoggedInMenu(const std::string& username, const std::string& userType, double balance);

    // 显示注册界面
    static void showRegisterForm();

    // 显示登录界面
    static void showLoginForm();

    // 显示账户管理菜单
    static void showAccountMenu();

    // 显示密码修改界面
    static void showChangePasswordForm();

    // 显示商品浏览界面
    static void showProductBrowseMenu();

    // 显示商品列表
    static void showProductList();

    // 显示商品搜索界面
    static void showProductSearchForm();

    // 显示商品详情界面
    static void showProductDetail();

    // 显示商家商品管理菜单
    static void showMerchantProductMenu();

    // 显示添加商品界面
    static void showAddProductForm();

    // 显示修改商品界面
    static void showModifyProductForm();

    // 显示商家商品列表
    static void showMerchantProductList();

    // 显示设置折扣界面
    static void showSetDiscountForm();

    // 显示购物车菜单（消费者）
    static void showCartMenu();

    // 显示购物车详情
    static void showCartDetail();

    // 显示修改购物车商品数量界面
    static void showUpdateCartItemForm();

    // 显示移除购物车商品界面
    static void showRemoveCartItemForm();

    // 显示消费者菜单
    static void showConsumerMenu();

    // 显示订单管理菜单
    static void showOrderMenu();

    // 显示订单列表
    static void showOrderList();

    // 显示订单详情
    static void showOrderDetail();

    // 显示退出信息
    static void showExitMessage();

    // 获取菜单选择
    static std::string getMenuChoice(int maxChoice);

    // 显示加载动画
    static void showLoading(const std::string& message);

    // 显示结算界面
    static void showCheckoutInterface();

    // 显示结算确认界面
    static void showCheckoutConfirmation();
};

#endif