#include "include.h"
#include "database_manager.h"

DatabaseManager* DatabaseManager::instance = nullptr;
const std::string DatabaseManager::DB_FILE = "ecommerce.db";

DatabaseManager::DatabaseManager() : db(nullptr){
  init();
}

DatabaseManager* DatabaseManager::getInstance(){
  if(instance == nullptr){
    instance = new DatabaseManager();
  }
  return instance;
}

void DatabaseManager::init(){
  if (sqlite3_open(DB_FILE.c_str(), &db) != SQLITE_OK) {
        throw std::runtime_error("无法打开数据库: " + std::string(sqlite3_errmsg(db)));
    }
    
    // 启用外键约束
    executeQuery("PRAGMA foreign_keys = ON;");
    
    // 创建数据库表
    const char* createTables[] = {
        "CREATE TABLE IF NOT EXISTS users ("
        "    username TEXT PRIMARY KEY,"
        "    password_hash TEXT NOT NULL,"
        "    user_type TEXT NOT NULL,"
        "    balance REAL DEFAULT 0.0 CHECK(balance >= 0)"
        ");",
        
        "CREATE TABLE IF NOT EXISTS products ("
        "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    name TEXT NOT NULL,"
        "    category TEXT NOT NULL,"
        "    description TEXT,"
        "    price REAL NOT NULL CHECK(price >= 0),"
        "    quantity INTEGER NOT NULL CHECK(quantity >= 0),"
        "    seller_username TEXT,"
        "    FOREIGN KEY(seller_username) REFERENCES users(username)"
        ");",
        
        "CREATE TABLE IF NOT EXISTS cart_items ("
        "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    user_username TEXT,"
        "    product_id INTEGER,"
        "    quantity INTEGER NOT NULL CHECK(quantity > 0),"
        "    FOREIGN KEY(user_username) REFERENCES users(username),"
        "    FOREIGN KEY(product_id) REFERENCES products(id)"
        ");"
    };
    
    for (const char* query : createTables) {
        executeQuery(query);
    }
}

void DatabaseManager::executeQuery(const std::string& query){
  char* errMsg = nullptr;
  if(sqlite3_exec(db, query.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK){
    std::string error = errMsg;
    sqlite3_free(errMsg);
    throw std::runtime_error("SQL错误：" + error);
  } 
}

void DatabaseManager::beginTransaction(){
  executeQuery("BEGIN TRANSACTION;");
}

void DatabaseManager::commit(){
  executeQuery("COMMIT;");
}

void DatabaseManager::rollback(){
  executeQuery("ROLLBACK;");
}

void DatabaseManager::close(){
  if(db){
    sqlite3_close(db);
    db = nullptr;
  }
}

DatabaseManager::~DatabaseManager(){
  close();
  if(instance == this){
    instance = nullptr;
  }
}