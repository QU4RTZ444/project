#include "user.h"
#include <iostream>

bool User::changePassword(const std::string& oldPassword, const std::string& newPassword) {
    if (password == oldPassword) {
        password = newPassword;
        return true;
    }
    return false;
}

void User::serialize(std::ofstream& out) const {
    // 写入用户类型
    int type = static_cast<int>(userType);
    out.write(reinterpret_cast<const char*>(&type), sizeof(type));

    // 写入用户名长度和内容
    size_t usernameLen = username.length();
    out.write(reinterpret_cast<const char*>(&usernameLen), sizeof(usernameLen));
    out.write(username.c_str(), usernameLen);

    // 写入密码长度和内容
    size_t passwordLen = password.length();
    out.write(reinterpret_cast<const char*>(&passwordLen), sizeof(passwordLen));
    out.write(password.c_str(), passwordLen);

    // 写入余额
    out.write(reinterpret_cast<const char*>(&balance), sizeof(balance));
}

void User::deserialize(std::ifstream& in) {
    // 读取用户类型
    int type;
    in.read(reinterpret_cast<char*>(&type), sizeof(type));
    userType = static_cast<UserType>(type);

    // 读取用户名
    size_t usernameLen;
    in.read(reinterpret_cast<char*>(&usernameLen), sizeof(usernameLen));
    username.resize(usernameLen);
    in.read(&username[0], usernameLen);

    // 读取密码
    size_t passwordLen;
    in.read(reinterpret_cast<char*>(&passwordLen), sizeof(passwordLen));
    password.resize(passwordLen);
    in.read(&password[0], passwordLen);

    // 读取余额
    in.read(reinterpret_cast<char*>(&balance), sizeof(balance));
}

User* User::createUser(const std::string& username, const std::string& password, UserType type) {
    if (type == UserType::CONSUMER) {
        return new Consumer(username, password);
    }
    else if (type == UserType::MERCHANT) {
        return new Merchant(username, password);
    }
    return nullptr;
}