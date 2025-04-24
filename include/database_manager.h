/*
@file database_manager.h
@author QU4RTZ444
@date 2025-04-24 23:26
 */
#ifndef database_manager_H
#define database_manager_H

#include "include.h"

class DatabaseManager {
private:
    sqlite3* db;
    static DatabaseManager* instance;
    
    DatabaseManager();  // 单例模式
    
public:
    static DatabaseManager* getInstance();
    void init();
    void close();
    void executeQuery(const std::string& query);
    
    // 防止复制
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;
    
    ~DatabaseManager();
};

#endif // database_manager_H