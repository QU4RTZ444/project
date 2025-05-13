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
};

#endif // database_manager_H