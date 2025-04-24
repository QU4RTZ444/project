/*
@file user.h
@author QU4RTZ444
@date 2025-04-17 16:30
 */
#ifndef user_H
#define user_H

#include "include.h"

class User {
  protected:
      std::string username;      // 用户名
      size_t passwordHash;       // 加密后的密码哈希值
      double balance;            // 账户余额
  
  public:
      // 构造函数：初始化用户名、密码（哈希后）、余额
      User(const std::string& uname, const std::string& pwd, double bal = 0.0)
          : username(uname), passwordHash(std::hash<std::string>{}(pwd)), balance(bal) {}
  
      virtual ~User() = default;
  
      // 获取用户类型的纯虚函数（必须在子类中实现）
      virtual std::string getUserType() const = 0;
  
      // 显示用户信息
      virtual void displayInfo() const {
          std::cout << "用户名: " << username << "\n"
                    << "账户类型: " << getUserType() << "\n"
                    << "余额: " << balance << "\n";
      }
  
      // 验证密码是否匹配（输入明文 -> 哈希后对比）
      bool authenticate(const std::string& pwd) const {
          return std::hash<std::string>{}(pwd) == passwordHash;
      }
  
      // 修改密码（重新哈希）
      void changePassword(const std::string& newPwd) {
          passwordHash = std::hash<std::string>{}(newPwd);
      }
  
      // 充值账户余额
      void recharge(double amount) {
          if (amount > 0) balance += amount;
      }
  
      // 扣除账户余额
      void deduct(double amount) {
          if (amount > 0 && balance >= amount) balance -= amount;
      }
  
      // 获取用户名
      const std::string& getUsername() const { return username; }
  
      // 获取账户余额
      double getBalance() const { return balance; }
  };

class Consumer : public User {
  public:
      Consumer(const std::string& uname, const std::string& pwd, double bal = 0.0)
          : User(uname, pwd, bal) {}
      std::string getUserType() const override;
};

class Seller : public User {
  public:
      Seller(const std::string& uname, const std::string& pwd, double bal = 0.0)
          : User(uname, pwd, bal) {}
      std::string getUserType() const override;
};


#endif // user_H