#ifndef USER_MANAGER_H
#define USER_MANAGER_H

#include "user.h"
#include <vector>
#include <string>
#include <mutex>
#include <memory>

class UserManager {
private:
    std::vector<std::unique_ptr<User>> users;
    std::string filename;
    std::mutex usersMutex;

    void loadUsers();

public:
    void saveUsers();
    UserManager(const std::string& filename);
    ~UserManager();

    bool registerUser(const std::string& username, const std::string& password, UserType userType);
    User* authenticateUser(const std::string& username, const std::string& password);
    bool userExists(const std::string& username);
    bool updateUser(const User& user);
    bool changePassword(const std::string& username, const std::string& oldPassword, const std::string& newPassword);
};

#endif