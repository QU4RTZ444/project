#include "user_manager.h"
#include <fstream>
#include <iostream>

UserManager::UserManager(const std::string& filename) : filename(filename) {
    loadUsers();
}

UserManager::~UserManager() {
    saveUsers();
}

bool UserManager::registerUser(const std::string& username, const std::string& password, UserType userType) {
    std::lock_guard<std::mutex> lock(usersMutex);

    // 检查用户名是否已存在
    for (const auto& user : users) {
        if (user->getUsername() == username) {
            std::cout << "用户名已存在: " << username << std::endl;
            return false;
        }
    }

    // 使用工厂方法创建新用户
    std::unique_ptr<User> newUser(User::createUser(username, password, userType));
    if (!newUser) {
        std::cout << "创建用户失败: " << username << std::endl;
        return false;
    }

    // 为消费者设置初始余额
    if (userType == UserType::CONSUMER) {
        newUser->setBalance(1000.0); // 给消费者1000元初始余额
    }

    users.push_back(std::move(newUser));
    saveUsers(); // 立即保存到文件

    std::cout << "用户注册成功: " << username << " (类型: " <<
        (userType == UserType::CONSUMER ? "消费者" : "商家") << ")" << std::endl;
    return true;
}

User* UserManager::authenticateUser(const std::string& username, const std::string& password) {
    std::lock_guard<std::mutex> lock(usersMutex);

    for (auto& user : users) {
        if (user->getUsername() == username && user->verifyPassword(password)) {
            std::cout << "用户登录验证成功: " << username << std::endl;
            return user.get();
        }
    }

    std::cout << "用户登录验证失败: " << username << std::endl;
    return nullptr;
}

bool UserManager::userExists(const std::string& username) {
    std::lock_guard<std::mutex> lock(usersMutex);

    for (const auto& user : users) {
        if (user->getUsername() == username) {
            return true;
        }
    }
    return false;
}

bool UserManager::updateUser(const User& updatedUser) {
    std::lock_guard<std::mutex> lock(usersMutex);

    for (auto& user : users) {
        if (user->getUsername() == updatedUser.getUsername()) {
            // 更新用户信息
            user->setBalance(updatedUser.getBalance());
            saveUsers();
            return true;
        }
    }
    return false;
}

bool UserManager::changePassword(const std::string& username, const std::string& oldPassword, const std::string& newPassword) {
    std::lock_guard<std::mutex> lock(usersMutex);

    for (auto& user : users) {
        if (user->getUsername() == username) {
            if (user->changePassword(oldPassword, newPassword)) {
                saveUsers(); // 保存更改
                std::cout << "用户 " << username << " 密码修改成功" << std::endl;
                return true;
            }
            else {
                std::cout << "用户 " << username << " 旧密码验证失败" << std::endl;
                return false;
            }
        }
    }

    std::cout << "用户 " << username << " 不存在" << std::endl;
    return false;
}

void UserManager::loadUsers() {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cout << "用户文件不存在，将创建新文件: " << filename << std::endl;
        return;
    }

    users.clear();

    try {
        size_t userCount;
        file.read(reinterpret_cast<char*>(&userCount), sizeof(userCount));

        for (size_t i = 0; i < userCount; ++i) {
            // 先读取用户类型来确定要创建哪种用户
            int type;
            file.read(reinterpret_cast<char*>(&type), sizeof(type));

            // 回退文件指针
            file.seekg(-static_cast<std::streamoff>(sizeof(type)), std::ios::cur);

            UserType userType = static_cast<UserType>(type);

            // 创建临时用户对象来读取数据
            std::unique_ptr<User> user(User::createUser("temp", "temp", userType));
            if (user) {
                user->deserialize(file);
                users.push_back(std::move(user));
            }
        }

        std::cout << "成功加载 " << users.size() << " 个用户" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "加载用户文件时出错: " << e.what() << std::endl;
        users.clear();
    }

    file.close();
}

void UserManager::saveUsers() {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "无法打开用户文件进行写入: " << filename << std::endl;
        return;
    }

    try {
        size_t userCount = users.size();
        file.write(reinterpret_cast<const char*>(&userCount), sizeof(userCount));

        for (const auto& user : users) {
            user->serialize(file);
        }

        std::cout << "成功保存 " << users.size() << " 个用户到文件" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "保存用户文件时出错: " << e.what() << std::endl;
    }

    file.close();
}