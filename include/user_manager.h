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

class UserManager {
private:
  DatabaseManager* db;

public:
  UserManager() : db(DatabaseManager::getInstance()) {}

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

  // 获取用户余额
  // @param uname: 用户名
  // @return: 返回用户余额
  double getBalance(const std::string& uname) const;

  // 更新用户余额
  // @param uname: 用户名
  // @param newBalance: 新的余额
  // @return: 更新成功返回true，失败返回false
  bool updateBalance(const std::string& uname, double newBalance);

  // 修改密码功能
  bool changePassword(const std::string& uname, const std::string& oldPwd, const std::string& newPwd);
};

#endif // user_manager_H