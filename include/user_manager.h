/*
@file user_manager.h
@author QU4RTZ444
@date 2025-04-17 16:47
 */
#ifndef user_manager_H
#define user_manager_H

#include "include.h"

class UserManager {
private:
    // 存储所有用户信息的哈希表
    // key: 用户名
    // value: 用户对象的智能指针
    std::unordered_map<std::string, std::shared_ptr<User>> users;

public:
    // 注册新用户
    // @param uname: 用户名
    // @param pwd: 密码
    // @param type: 用户类型("消费者"或"商家")
    // @return: 注册成功返回true，失败返回false
    bool registerUser(const std::string& uname, const std::string& pwd, const std::string& type);

    // 用户登录验证
    // @param uname: 用户名
    // @param pwd: 密码
    // @return: 登录成功返回用户对象指针，失败返回nullptr
    std::shared_ptr<User> login(const std::string& uname, const std::string& pwd);

    // 将用户数据保存到文件
    void saveToFile() const;

    // 从文件加载用户数据
    void loadFromFile();
};

#endif // user_manager_H