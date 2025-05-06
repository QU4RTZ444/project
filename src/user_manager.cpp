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
        
        // 绑定参数
        sqlite3_bind_text(stmt, 1, uname.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, std::to_string(std::hash<std::string>{}(pwd)).c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, type.c_str(), -1, SQLITE_STATIC);
        
        // 执行语句
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
        
        if (storedHash == std::to_string(std::hash<std::string>{}(pwd))) {
            sqlite3_finalize(stmt);
            if (storedType == "Consumer") {
                return std::make_shared<Consumer>(uname, pwd);
            } else {
                return std::make_shared<Seller>(uname, pwd);
            }
        }
    }
    
    sqlite3_finalize(stmt);
    return nullptr;
}