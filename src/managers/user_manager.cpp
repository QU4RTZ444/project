#include "include.h"

bool UserManager::registerUser(const std::string& uname, const std::string& pwd, const std::string& type) {
    try {
        std::string query = "INSERT INTO users (username, password_hash, user_type, balance) "
                           "VALUES (?, ?, ?, 0.0);";
        
        sqlite3_stmt* stmt;
        sqlite3* dbHandle = db->getHandle();
        
        if (sqlite3_prepare_v2(dbHandle, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            return false;
        }
        
        // 计算密码哈希
        size_t pwdHash = std::hash<std::string>{}(pwd);
        std::string hashStr = std::to_string(pwdHash);
        
        // 绑定参数
        sqlite3_bind_text(stmt, 1, uname.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, hashStr.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, type.c_str(), -1, SQLITE_STATIC);
        
        int result = sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        
        return result == SQLITE_DONE;
    } catch (const std::exception& e) {
        return false;
    }
}

std::shared_ptr<User> UserManager::login(const std::string& uname, const std::string& pwd) {
    std::string query = "SELECT user_type, password_hash FROM users WHERE username = ?;";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db->getHandle(), query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return nullptr;
    }
    
    sqlite3_bind_text(stmt, 1, uname.c_str(), -1, SQLITE_STATIC);
    
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        std::string storedType = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        std::string storedHash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        
        // 计算输入密码的哈希并转换为字符串
        size_t inputHash = std::hash<std::string>{}(pwd);
        std::string inputHashStr = std::to_string(inputHash);
        
        // 比较哈希字符串
        if (storedHash == inputHashStr) {
            sqlite3_finalize(stmt);
            if (storedType == "消费者") {
                return std::make_shared<Consumer>(uname, pwd);
            } else {
                return std::make_shared<Seller>(uname, pwd);
            }
        }
    }
    
    sqlite3_finalize(stmt);
    return nullptr;
}

double UserManager::getBalance(const std::string& uname) const {
    std::string query = "SELECT balance FROM users WHERE username = ?;";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db->getHandle(), query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return -1.0;  // 表示查询失败
    }
    
    sqlite3_bind_text(stmt, 1, uname.c_str(), -1, SQLITE_STATIC);
    
    double balance = -1.0;  // 默认值表示未找到用户
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        balance = sqlite3_column_double(stmt, 0);
    }
    
    sqlite3_finalize(stmt);
    return balance;
}

bool UserManager::updateBalance(const std::string& uname, double newBalance) {
    if (newBalance < 0) {
        return false;  // 余额不能为负
    }

    std::string query = "UPDATE users SET balance = ? WHERE username = ?;";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db->getHandle(), query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }
    
    sqlite3_bind_double(stmt, 1, newBalance);
    sqlite3_bind_text(stmt, 2, uname.c_str(), -1, SQLITE_STATIC);
    
    int result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    return result == SQLITE_DONE;
}

bool UserManager::changePassword(const std::string& uname, const std::string& oldPwd, const std::string& newPwd) {
    // 首先验证旧密码
    std::string query = "SELECT password_hash FROM users WHERE username = ?;";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db->getHandle(), query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }
    
    sqlite3_bind_text(stmt, 1, uname.c_str(), -1, SQLITE_STATIC);
    
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        std::string storedHash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        std::string oldHash = std::to_string(std::hash<std::string>{}(oldPwd));
        
        if (storedHash != oldHash) {
            sqlite3_finalize(stmt);
            return false;  // 旧密码错误
        }
    }
    sqlite3_finalize(stmt);
    
    // 更新新密码
    query = "UPDATE users SET password_hash = ? WHERE username = ?;";
    if (sqlite3_prepare_v2(db->getHandle(), query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }
    
    std::string newHash = std::to_string(std::hash<std::string>{}(newPwd));
    sqlite3_bind_text(stmt, 1, newHash.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, uname.c_str(), -1, SQLITE_STATIC);
    
    int result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    return result == SQLITE_DONE;
}