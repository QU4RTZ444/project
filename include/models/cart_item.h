/*
@file cart_item.h
@author QU4RTZ444
@date 2025-05-21 10:30
 */
#ifndef cart_item_H
#define cart_item_H

#include "include.h"

/**
 * @brief 购物车项目类
 * @details 表示购物车中的单个商品及其数量
 */
class CartItem {
private:
    /** @brief 商品指针 */
    std::shared_ptr<Product> product;
    /** @brief 商品数量 */
    int quantity;

public:
    /**
     * @brief 构造函数
     * @param prod 商品指针
     * @param qty 数量
     */
    CartItem(const std::shared_ptr<Product>& prod, int qty)
        : product(prod), quantity(qty) {}

    // Getters
    std::shared_ptr<Product> getProduct() const { return product; }
    int getQuantity() const { return quantity; }
    void setQuantity(int qty) { quantity = qty; }
};



#endif // cart_item_H