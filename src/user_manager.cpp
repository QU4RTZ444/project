#include "include.h"
#include "user_manager.h"

bool UserManager::registerUser(const std::string &uname, const std::string &pwd, const std::string &type){
  if (users.find(uname) != users.end()) {
        return false; // 用户名已存在
    }

    std::shared_ptr<User> newUser;
    if (type == "消费者") {
        newUser = std::make_shared<Consumer>(uname, pwd);
    } else if (type == "商家") {
        newUser = std::make_shared<Seller>(uname, pwd);
    } else {
        return false;
    }

    users[uname] = newUser;
    saveToFile(); // 保存到文件
    return true;
}