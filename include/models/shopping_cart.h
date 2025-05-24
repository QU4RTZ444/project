#ifndef SHOPPING_CART_H
#define SHOPPING_CART_H

#include "include.h"
#include "product.h"


/**
 * @brief 购物车类
 * @details 管理用户的购物车，包括添加、删除、修改商品等功能
 */
class ShoppingCart {
private:
    /** @brief 购物车中的商品列表 */
    std::vector<CartItem> items;
    /** @brief 用户名 */
    std::string username;

public:
    /**
     * @brief 构造函数
     * @param uname 用户名
     */
    explicit ShoppingCart(const std::string& uname) : username(uname) {}

    /**
     * @brief 添加商品到购物车
     * @param product 商品指针
     * @param quantity 数量
     * @return bool 添加是否成功
     * @throw std::runtime_error 如果数量无效或商品不可用
     */
    bool addItem(const std::shared_ptr<Product>& product, int quantity);

    /**
     * @brief 从购物车移除商品
     * @param productId 商品ID
     * @return bool 移除是否成功
     */
    bool removeItem(int productId);

    /**
     * @brief 修改商品数量
     * @param productId 商品ID
     * @param newQuantity 新数量
     * @return bool 修改是否成功
     * @throw std::runtime_error 如果数量无效
     */
    bool updateQuantity(int productId, int newQuantity);

    /**
     * @brief 清空购物车
     */
    void clear();

    /**
     * @brief 获取购物车所有商品
     * @return const vector<CartItem>& 商品列表
     */
    const std::vector<CartItem>& getItems() const { return items; }

    /**
     * @brief 计算购物车总金额
     * @return double 总金额
     */
    double calculateTotal() const;

    /**
     * @brief 检查商品是否在购物车中
     * @param productId 商品ID
     * @return bool 是否存在
     */
    bool containsProduct(int productId) const;

    /**
     * @brief 获取用户名
     * @return const string& 用户名
     */
    const std::string& getUsername() const { return username; }
};

#endif // SHOPPING_CART_H