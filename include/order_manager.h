#ifndef ORDER_MANAGER_H
#define ORDER_MANAGER_H

#include "include.h"
#include "order.h"
#include "database_manager.h"

/**
 * @brief 订单管理类
 * @details 负责订单的创建、查询、支付等管理功能
 */
class OrderManager {
private:
    /** @brief 数据库管理器指针 */
    DatabaseManager* db;

public:
    /**
     * @brief 构造函数
     */
    OrderManager() : db(DatabaseManager::getInstance()) {}

    /**
     * @brief 从购物车创建订单
     * @param cart 购物车对象
     * @return shared_ptr<Order> 创建的订单
     * @throw std::runtime_error 如果创建失败
     */
    std::shared_ptr<Order> createOrder(const ShoppingCart& cart);

    /**
     * @brief 支付订单
     * @param orderId 订单ID
     * @param username 用户名
     * @return bool 支付是否成功
     * @throw std::runtime_error 如果支付失败
     */
    bool payOrder(int orderId, const std::string& username);

    /**
     * @brief 取消订单
     * @param orderId 订单ID
     * @param username 用户名
     * @return bool 取消是否成功
     */
    bool cancelOrder(int orderId, const std::string& username);

    /**
     * @brief 获取用户的所有订单
     * @param username 用户名
     * @return vector<shared_ptr<Order>> 订单列表
     */
    std::vector<std::shared_ptr<Order>> getUserOrders(const std::string& username) const;

    /**
     * @brief 获取订单详情
     * @param orderId 订单ID
     * @return shared_ptr<Order> 订单对象
     */
    std::shared_ptr<Order> getOrder(int orderId) const;
};

#endif // ORDER_MANAGER_H