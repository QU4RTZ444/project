#ifndef USER_H
#define USER_H

#include <string>
#include <fstream>
#include <iostream>

// �û�����ö��
enum class UserType {
    CONSUMER = 1,  // ������
    MERCHANT = 2   // �̼�
};

// �����û�����
class User {
protected:
    std::string username;     // �û���
    std::string password;     // ����
    double balance;          // �˻����
    UserType userType;       // �û�����

public:
    // ���캯��
    User(const std::string& username, const std::string& password, UserType type)
        : username(username), password(password), balance(0.0), userType(type) {}

    // ����������
    virtual ~User() = default;

    // ��������
    const std::string& getUsername() const { return username; }
    bool verifyPassword(const std::string& pwd) const { return password == pwd; }
    double getBalance() const { return balance; }
    void setBalance(double newBalance) { balance = newBalance; }
    UserType getUserType() const { return userType; }

    // �������
    bool changePassword(const std::string& oldPassword, const std::string& newPassword);
    void setPassword(const std::string& newPassword) { password = newPassword; }

    // �麯��
    virtual void displayInfo() const = 0;
    virtual bool canSell() const = 0;
    virtual bool canBuy() const = 0;

    // ���л����� - ʹ�ö������ļ���
    void serialize(std::ofstream& out) const;
    void deserialize(std::ifstream& in);

    // ��̬��������
    static User* createUser(const std::string& username, const std::string& password, UserType type);
};

// ��������
class Consumer : public User {
public:
    Consumer(const std::string& username, const std::string& password)
        : User(username, password, UserType::CONSUMER) {}

    // ʵ�ִ��麯��
    void displayInfo() const override {
        std::cout << "�û�����: ������\n�û���: " << username << "\n���: " << balance << std::endl;
    }

    bool canSell() const override { return false; }
    bool canBuy() const override { return true; }
};

// �̼���
class Merchant : public User {
public:
    Merchant(const std::string& username, const std::string& password)
        : User(username, password, UserType::MERCHANT) {}

    // ʵ�ִ��麯��
    void displayInfo() const override {
        std::cout << "�û�����: �̼�\n�û���: " << username << "\n���: " << balance << std::endl;
    }

    bool canSell() const override { return true; }
    bool canBuy() const override { return true; }
};

#endif