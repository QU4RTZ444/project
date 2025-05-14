/*
@file database_manager.h
@author QU4RTZ444
@date 2025-04-24 23:26
 */
#ifndef database_manager_H
#define database_manager_H

#include "include.h"

class DatabaseManager {
private:
    sqlite3* db;
    static DatabaseManager* instance;
    static const std::string DB_FILE;
    
    /**
     * @brief 私有构造函数，实现单例模式
     * @details 初始化数据库连接句柄为空
     */
    DatabaseManager();

public:
    /**
     * @brief 获取数据库管理器的单例实例
     * @return DatabaseManager* 返回单例实例的指针
     */
    static DatabaseManager* getInstance();

    /**
     * @brief 初始化数据库连接
     * @throw std::runtime_error 如果数据库连接失败
     */
    void init();

    /**
     * @brief 执行SQL查询
     * @param query SQL查询语句
     * @throw std::runtime_error 如果查询执行失败
     */
    void executeQuery(const std::string& query);

    /**
     * @brief 开始数据库事务
     * @throw std::runtime_error 如果开启事务失败
     */
    void beginTransaction();

    /**
     * @brief 提交数据库事务
     * @throw std::runtime_error 如果提交事务失败
     */
    void commit();

    /**
     * @brief 回滚数据库事务
     * @throw std::runtime_error 如果回滚事务失败
     */
    void rollback();

    /**
     * @brief 关闭数据库连接
     * @throw std::runtime_error 如果关闭连接失败
     */
    void close();

    /**
     * @brief 获取数据库连接句柄
     * @return sqlite3* 返回SQLite数据库连接句柄
     */
    sqlite3* getHandle() { return db; }

    /**
     * @brief 禁用拷贝构造函数
     */
    DatabaseManager(const DatabaseManager&) = delete;

    /**
     * @brief 禁用赋值运算符
     */
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    /**
     * @brief 析构函数
     * @details 关闭数据库连接并释放资源
     */
    ~DatabaseManager();

    /**
     * @brief 保存购物车项目
     * @param username 用户名
     * @param productId 商品ID
     * @param quantity 数量
     * @return bool 操作是否成功
     */
    bool saveCartItem(const std::string& username, int productId, int quantity);

    /**
     * @brief 删除购物车项目
     * @param username 用户名
     * @param productId 商品ID
     * @return bool 操作是否成功
     */
    bool removeCartItem(const std::string& username, int productId);

    /**
     * @brief 获取用户的购物车
     * @param username 用户名
     * @return vector<pair<int, int>> 商品ID和数量的列表
     */
    std::vector<std::pair<int, int>> getCartItems(const std::string& username);

    /**
     * @brief 创建新订单
     * @param buyerUsername 买家用户名
     * @param totalAmount 总金额
     * @return int 新订单ID，失败返回-1
     */
    int createOrder(const std::string& buyerUsername, double totalAmount);

    /**
     * @brief 添加订单项目
     * @param orderId 订单ID
     * @param productId 商品ID
     * @param quantity 数量
     * @param price 价格
     * @param sellerUsername 卖家用户名
     * @return bool 操作是否成功
     */
    bool addOrderItem(int orderId, int productId, int quantity, 
                     double price, const std::string& sellerUsername);

    /**
     * @brief 锁定商品库存
     * @param productId 商品ID
     * @param orderId 订单ID
     * @param quantity 锁定数量
     * @return bool 操作是否成功
     */
    bool lockProductStock(int productId, int orderId, int quantity);

    /**
     * @brief 解锁商品库存
     * @param productId 商品ID
     * @param orderId 订单ID
     * @return bool 操作是否成功
     */
    bool unlockProductStock(int productId, int orderId);

    /**
     * @brief 更新订单状态
     * @param orderId 订单ID
     * @param status 新状态
     * @return bool 操作是否成功
     */
    bool updateOrderStatus(int orderId, const std::string& status);
};

#endif // database_manager_H