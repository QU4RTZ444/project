/*
@file user.h
@author QU4RTZ444
@date 2025-04-17 16:30
 */
#ifndef user_H
#define user_H

#include "include.h"

/**
 * @brief 用户抽象基类
 * @details 定义了用户的基本属性和操作，作为消费者和商家类的基类
 */
class User {
protected:
    /** @brief 用户名，作为用户的唯一标识符 */
    std::string username;
    /** @brief 密码的哈希值，用于安全存储密码 */
    size_t passwordHash;
    /** @brief 用户账户余额，用于支付和收款 */
    double balance;

public:
    /**
     * @brief 构造函数
     * @param uname 用户名
     * @param pwd 明文密码(将被哈希处理)
     * @param bal 初始余额，默认为0
     */
    User(const std::string& uname, const std::string& pwd, double bal = 0.0)
        : username(uname), passwordHash(std::hash<std::string>{}(pwd)), balance(bal) {}

    /**
     * @brief 虚析构函数
     * @details 使User成为抽象类，确保正确释放派生类对象
     */
    virtual ~User() = default;

    /**
     * @brief 获取用户类型
     * @return string 返回用户类型的字符串表示("商家"或"消费者")
     */
    virtual std::string getUserType() const = 0;

    /**
     * @brief 显示用户信息
     * @details 显示用户名、类型和余额等基本信息
     */
    virtual void displayInfo() const {
        std::cout << "用户名: " << username << "\n"
                 << "账户类型: " << getUserType() << "\n"
                 << "余额: " << balance << "\n";
    }

    /**
     * @brief 密码验证
     * @param pwd 待验证的明文密码
     * @return bool 密码匹配返回true，否则返回false
     */
    bool authenticate(const std::string& pwd) const {
        return std::hash<std::string>{}(pwd) == passwordHash;
    }

    /**
     * @brief 修改用户密码
     * @param newPwd 新密码（明文，将被哈希处理）
     */
    void changePassword(const std::string& newPwd) {
        passwordHash = std::hash<std::string>{}(newPwd);
    }

    /**
     * @brief 账户充值
     * @param amount 充值金额
     * @details 只接受正数金额
     */
    void recharge(double amount) {
        if (amount > 0) balance += amount;
    }

    /**
     * @brief 扣除账户余额
     * @param amount 扣除金额
     * @details 只在余额充足且金额为正数时执行扣除操作
     */
    void deduct(double amount) {
        if (amount > 0 && balance >= amount) balance -= amount;
    }

    /**
     * @brief 获取用户名
     * @return const string& 用户名
     */
    const std::string& getUsername() const { return username; }

    /**
     * @brief 获取账户余额
     * @return double 当前账户余额
     */
    double getBalance() const { return balance; }
};

/**
 * @brief 消费者用户类
 * @details 继承自User基类的消费者类型用户
 */
class Consumer : public User {
public:
    /**
     * @brief 消费者类构造函数
     * @param uname 用户名
     * @param pwd 密码
     * @param bal 初始余额，默认为0
     */
    Consumer(const std::string& uname, const std::string& pwd, double bal = 0.0)
        : User(uname, pwd, bal) {}

    /**
     * @brief 获取用户类型
     * @return string 返回"消费者"
     */
    std::string getUserType() const override;
};

/**
 * @brief 商家用户类
 * @details 继承自User基类的商家类型用户
 */
class Seller : public User {
public:
    /**
     * @brief 商家类构造函数
     * @param uname 用户名
     * @param pwd 密码
     * @param bal 初始余额，默认为0
     */
    Seller(const std::string& uname, const std::string& pwd, double bal = 0.0)
        : User(uname, pwd, bal) {}

    /**
     * @brief 获取用户类型
     * @return string 返回"商家"
     */
    std::string getUserType() const override;
};

#endif // user_H