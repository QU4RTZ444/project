#ifndef CART_MANAGER_H
#define CART_MANAGER_H

#include "cart.h"
#include <map>
#include <mutex>
#include <string>

class CartManager {
private:
    std::map<std::string, Cart> userCarts; // username -> Cart
    mutable std::mutex cartsMutex;
    std::string filename;

    void loadCarts();
    void saveCarts();

public:
    CartManager(const std::string& filename);
    ~CartManager();

    // 添加商品到用户购物车
    bool addItemToCart(const std::string& username, const CartItem& item);

    // 更新购物车中商品数量
    bool updateCartItem(const std::string& username, int productId, int quantity);

    // 从购物车移除商品
    bool removeItemFromCart(const std::string& username, int productId);

    // 清空用户购物车
    bool clearUserCart(const std::string& username);

    // 获取用户购物车
    Cart getUserCart(const std::string& username) const;

    // 获取用户购物车中的商品列表
    std::vector<CartItem> getUserCartItems(const std::string& username) const;

    // 获取用户购物车总价
    double getUserCartTotalPrice(const std::string& username) const;

    // 获取用户购物车商品数量
    int getUserCartItemCount(const std::string& username) const;
};

#endif