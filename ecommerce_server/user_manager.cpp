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

    // ����û����Ƿ��Ѵ���
    for (const auto& user : users) {
        if (user->getUsername() == username) {
            std::cout << "�û����Ѵ���: " << username << std::endl;
            return false;
        }
    }

    // ʹ�ù��������������û�
    std::unique_ptr<User> newUser(User::createUser(username, password, userType));
    if (!newUser) {
        std::cout << "�����û�ʧ��: " << username << std::endl;
        return false;
    }

    // Ϊ���������ó�ʼ���
    if (userType == UserType::CONSUMER) {
        newUser->setBalance(1000.0); // ��������1000Ԫ��ʼ���
    }

    users.push_back(std::move(newUser));
    saveUsers(); // �������浽�ļ�

    std::cout << "�û�ע��ɹ�: " << username << " (����: " <<
        (userType == UserType::CONSUMER ? "������" : "�̼�") << ")" << std::endl;
    return true;
}

User* UserManager::authenticateUser(const std::string& username, const std::string& password) {
    std::lock_guard<std::mutex> lock(usersMutex);

    for (auto& user : users) {
        if (user->getUsername() == username && user->verifyPassword(password)) {
            std::cout << "�û���¼��֤�ɹ�: " << username << std::endl;
            return user.get();
        }
    }

    std::cout << "�û���¼��֤ʧ��: " << username << std::endl;
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
            // �����û���Ϣ
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
                saveUsers(); // �������
                std::cout << "�û� " << username << " �����޸ĳɹ�" << std::endl;
                return true;
            }
            else {
                std::cout << "�û� " << username << " ��������֤ʧ��" << std::endl;
                return false;
            }
        }
    }

    std::cout << "�û� " << username << " ������" << std::endl;
    return false;
}

void UserManager::loadUsers() {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cout << "�û��ļ������ڣ����������ļ�: " << filename << std::endl;
        return;
    }

    users.clear();

    try {
        size_t userCount;
        file.read(reinterpret_cast<char*>(&userCount), sizeof(userCount));

        for (size_t i = 0; i < userCount; ++i) {
            // �ȶ�ȡ�û�������ȷ��Ҫ���������û�
            int type;
            file.read(reinterpret_cast<char*>(&type), sizeof(type));

            // �����ļ�ָ��
            file.seekg(-static_cast<std::streamoff>(sizeof(type)), std::ios::cur);

            UserType userType = static_cast<UserType>(type);

            // ������ʱ�û���������ȡ����
            std::unique_ptr<User> user(User::createUser("temp", "temp", userType));
            if (user) {
                user->deserialize(file);
                users.push_back(std::move(user));
            }
        }

        std::cout << "�ɹ����� " << users.size() << " ���û�" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "�����û��ļ�ʱ����: " << e.what() << std::endl;
        users.clear();
    }

    file.close();
}

void UserManager::saveUsers() {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "�޷����û��ļ�����д��: " << filename << std::endl;
        return;
    }

    try {
        size_t userCount = users.size();
        file.write(reinterpret_cast<const char*>(&userCount), sizeof(userCount));

        for (const auto& user : users) {
            user->serialize(file);
        }

        std::cout << "�ɹ����� " << users.size() << " ���û����ļ�" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "�����û��ļ�ʱ����: " << e.what() << std::endl;
    }

    file.close();
}