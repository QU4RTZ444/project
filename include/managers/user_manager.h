/*
@file user_manager.h
@author QU4RTZ444
@date 2025-04-17 16:47
 */
#ifndef user_manager_H
#define user_manager_H

#include "include.h"

class User;
class DatabaseManager;

/**
 * @brief 用户管理类
 * @details 负责用户账户的管理，包括注册、登录、余额管理和密码修改等功能
 */
class UserManager {
private:
    /** @brief 数据库管理器指针 */
    DatabaseManager* db;

public:
    /**
     * @brief 构造函数
     * @details 初始化数据库连接
     */
    UserManager() : db(DatabaseManager::getInstance()) {}

    /**
     * @brief 注册新用户
     * @param uname 用户名
     * @param pwd 密码
     * @param type 用户类型("消费者"或"商家")
     * @return bool 注册成功返回true，失败返回false
     * @throw std::runtime_error 如果用户名已存在或参数无效
     */
    bool registerUser(const std::string& uname, const std::string& pwd, const std::string& type);

    /**
     * @brief 用户登录验证
     * @param uname 用户名
     * @param pwd 密码
     * @return shared_ptr<User> 登录成功返回用户对象指针，失败返回nullptr
     * @throw std::runtime_error 如果数据库访问失败
     */
    std::shared_ptr<User> login(const std::string& uname, const std::string& pwd);

    /**
     * @brief 获取用户余额
     * @param uname 用户名
     * @return double 返回用户余额
     * @throw std::runtime_error 如果用户不存在或数据库访问失败
     */
    double getBalance(const std::string& uname) const;

    /**
     * @brief 更新用户余额
     * @param uname 用户名
     * @param newBalance 新的余额
     * @return bool 更新成功返回true，失败返回false
     * @throw std::runtime_error 如果用户不存在或余额无效
     */
    bool updateBalance(const std::string& uname, double newBalance);

    /**
     * @brief 修改用户密码
     * @param uname 用户名
     * @param oldPwd 原密码
     * @param newPwd 新密码
     * @return bool 修改成功返回true，失败返回false
     * @throw std::runtime_error 如果原密码错误或用户不存在
     */
    bool changePassword(const std::string& uname, const std::string& oldPwd, const std::string& newPwd);
};

#endif // user_manager_H