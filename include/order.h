#ifndef ORDER_H
#define ORDER_H

#include "include.h"
#include "shopping_cart.h"

/**
 * @brief 订单状态枚举
 */
enum class OrderStatus {
    PENDING,    // 待支付
    PAID,       // 已支付
    CANCELLED,  // 已取消
    FAILED      // 失败
};

/**
 * @brief 订单类
 * @details 管理订单信息，包括订单状态、商品列表、支付等功能
 */
class Order {
private:
    /** @brief 订单ID */
    int id;
    /** @brief 买家用户名 */
    std::string buyerUsername;
    /** @brief 订单商品列表 */
    std::vector<CartItem> items;
    /** @brief 订单总金额 */
    double totalAmount;
    /** @brief 订单状态 */
    OrderStatus status;
    /** @brief 创建时间 */
    std::string createTime;

public:
    /**
     * @brief 构造函数
     * @param orderId 订单ID
     * @param username 用户名
     * @param cartItems 购物车商品
     */
    Order(int orderId, const std::string& username, const std::vector<CartItem>& cartItems);

    /**
     * @brief 支付订单
     * @return bool 支付是否成功
     * @throw std::runtime_error 如果支付失败
     */
    bool pay();

    /**
     * @brief 取消订单
     * @return bool 取消是否成功
     */
    bool cancel();

    // Getters
    int getId() const { return id; }
    const std::string& getBuyerUsername() const { return buyerUsername; }
    OrderStatus getStatus() const { return status; }
    double getTotalAmount() const { return totalAmount; }
    const std::vector<CartItem>& getItems() const { return items; }
    const std::string& getCreateTime() const { return createTime; }
};

#endif // ORDER_H