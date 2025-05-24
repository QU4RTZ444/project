#ifndef MENU_HANDLER_H
#define MENU_HANDLER_H

#include "menu_handler_base.h"
#include "cart_handler.h"
#include "order_handler.h"
#include "product_handler_base.h"
#include "user_handler.h"

/**
 * @brief 菜单处理类
 * @details 负责处理所有菜单相关的交互操作，包括用户操作和商品管理
 */
class MenuHandler : public MenuHandlerBase {
private:
    /** @brief 购物车处理器 */
    CartHandler cartHandler;

    /** @brief 订单处理器 */
    OrderHandler orderHandler;

    /** @brief 商品处理器 */
    ProductHandler productHandler;

    /** @brief 用户处理器 */
    UserHandler userHandler;

public:
    /**
     * @brief 构造函数
     * @param um 用户管理器
     * @param pm 商品管理器
     */
    MenuHandler(UserManager& um, ProductManager& pm)
        : MenuHandlerBase(um, pm)
        , cartHandler(um, pm)
        , orderHandler(um, pm)
        , productHandler(um, pm, cartHandler)  // 传入 cartHandler 引用
        , userHandler(um, pm) {}

    /**
     * @brief 显示用户菜单
     * @param currentUser 当前登录的用户
     */
    void displayUserMenu(const std::shared_ptr<User>& currentUser);

    /**
     * @brief 处理消费者的菜单选择
     * @param currentUser 当前登录的用户
     * @return bool 如果用户选择退出返回true，否则返回false
     */
    bool handleConsumerChoice(const std::shared_ptr<User>& currentUser);
    
    /**
     * @brief 处理商家的菜单选择
     * @param currentUser 当前登录的用户
     * @return bool 如果用户选择退出返回true，否则返回false
     */
    bool handleSellerChoice(const std::shared_ptr<User>& currentUser);

    /**
     * @brief 获取购物车处理器
     * @return CartHandler& 购物车处理器引用
     */
    CartHandler& getCartHandler() { return cartHandler; }

    /**
     * @brief 获取订单处理器
     * @return OrderHandler& 订单处理器引用
     */
    OrderHandler& getOrderHandler() { return orderHandler; }

    /**
     * @brief 获取商品处理器
     * @return ProductHandler& 商品处理器引用
     */
    ProductHandler& getProductHandler() { return productHandler; }

    /**
     * @brief 获取用户处理器
     * @return UserHandler& 用户处理器引用
     */
    UserHandler& getUserHandler() { return userHandler; }
};

#endif