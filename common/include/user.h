#ifndef USER_H
#define USER_H

#include <string>
#include <fstream>
#include <iostream>

// 用户类型枚举
enum class UserType {
    CONSUMER = 1,  // 消费者
    MERCHANT = 2   // 商家
};

// 抽象用户基类
class User {
protected:
    std::string username;     // 用户名
    std::string password;     // 密码
    double balance;          // 账户余额
    UserType userType;       // 用户类型

public:
    // 构造函数
    User(const std::string& username, const std::string& password, UserType type)
        : username(username), password(password), balance(0.0), userType(type) {}

    // 虚析构函数
    virtual ~User() = default;

    // 基本功能
    const std::string& getUsername() const { return username; }
    bool verifyPassword(const std::string& pwd) const { return password == pwd; }
    double getBalance() const { return balance; }
    void setBalance(double newBalance) { balance = newBalance; }
    UserType getUserType() const { return userType; }

    // 密码管理
    bool changePassword(const std::string& oldPassword, const std::string& newPassword);
    void setPassword(const std::string& newPassword) { password = newPassword; }

    // 虚函数
    virtual void displayInfo() const = 0;
    virtual bool canSell() const = 0;
    virtual bool canBuy() const = 0;

    // 序列化方法 - 使用二进制文件流
    void serialize(std::ofstream& out) const;
    void deserialize(std::ifstream& in);

    // 静态工厂方法
    static User* createUser(const std::string& username, const std::string& password, UserType type);
};

// 消费者类
class Consumer : public User {
public:
    Consumer(const std::string& username, const std::string& password)
        : User(username, password, UserType::CONSUMER) {}

    // 实现纯虚函数
    void displayInfo() const override {
        std::cout << "用户类型: 消费者\n用户名: " << username << "\n余额: " << balance << std::endl;
    }

    bool canSell() const override { return false; }
    bool canBuy() const override { return true; }
};

// 商家类
class Merchant : public User {
public:
    Merchant(const std::string& username, const std::string& password)
        : User(username, password, UserType::MERCHANT) {}

    // 实现纯虚函数
    void displayInfo() const override {
        std::cout << "用户类型: 商家\n用户名: " << username << "\n余额: " << balance << std::endl;
    }

    bool canSell() const override { return true; }
    bool canBuy() const override { return true; }
};

#endif