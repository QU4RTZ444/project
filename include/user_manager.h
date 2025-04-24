/*
@file user_manager.h
@author QU4RTZ444
@date 2025-04-17 16:47
 */
#ifndef user_manager_H
#define user_manager_H

#include "include.h"

class UserManager {
  private:
      std::unordered_map<std::string, std::shared_ptr<User>> users;
  
  public:
      bool registerUser(const std::string& uname, const std::string& pwd, const std::string& type);
      std::shared_ptr<User> login(const std::string& uname, const std::string& pwd);
      void saveToFile() const;
      void loadFromFile();
};

#endif // user_manager_H