#include "include.h"
#include "database_manager.h"
#include <fstream>
#include <nlohmann/json.hpp>

DatabaseManager* DatabaseManager::instance = nullptr;
const std::string DatabaseManager::DB_FILE = "ecommerce.db";

DatabaseManager::DatabaseManager() : db(nullptr), inTransaction(false) {
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
    };
    
    for (const char* query : createTables) {
        executeQuery(query);
    }

    // 检查并添加 discount_rate 列
    const char* alterTable = 
        "ALTER TABLE products ADD COLUMN discount_rate REAL "
        "DEFAULT 1.0 CHECK(discount_rate > 0 AND discount_rate <= 1);";
    
    try {
        executeQuery(alterTable);
    } catch (const std::exception& e) {
        // 如果列已存在，SQLite 会抛出错误，我们可以忽略它
        #ifdef DEBUG
        std::cout << "添加 discount_rate 列时出错(可能列已存在): " << e.what() << "\n";
        #endif
    }

    // 创建购物车表
    executeQuery(R"(
        CREATE TABLE IF NOT EXISTS shopping_cart (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            username TEXT NOT NULL,
            product_id INTEGER NOT NULL,
            quantity INTEGER NOT NULL,
            FOREIGN KEY (username) REFERENCES users(username),
            FOREIGN KEY (product_id) REFERENCES products(id)
        )
    )");

    // 创建订单表
    executeQuery(R"(
        CREATE TABLE IF NOT EXISTS orders (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            buyer_username TEXT NOT NULL,
            total_amount REAL NOT NULL,
            status TEXT NOT NULL,
            create_time DATETIME DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY (buyer_username) REFERENCES users(username)
        )
    )");

    // 创建订单项目表
    executeQuery(R"(
        CREATE TABLE IF NOT EXISTS order_items (
            order_id INTEGER NOT NULL,
            product_id INTEGER NOT NULL,
            quantity INTEGER NOT NULL,
            price REAL NOT NULL,
            seller_username TEXT NOT NULL,
            PRIMARY KEY (order_id, product_id),
            FOREIGN KEY (order_id) REFERENCES orders(id),
            FOREIGN KEY (product_id) REFERENCES products(id),
            FOREIGN KEY (seller_username) REFERENCES users(username)
        )
    )");

    // 创建商品库存锁定表
    executeQuery(R"(
        CREATE TABLE IF NOT EXISTS product_locks (
            product_id INTEGER NOT NULL,
            order_id INTEGER NOT NULL,
            locked_quantity INTEGER NOT NULL,
            lock_time DATETIME DEFAULT CURRENT_TIMESTAMP,
            PRIMARY KEY (product_id, order_id),
            FOREIGN KEY (product_id) REFERENCES products(id),
            FOREIGN KEY (order_id) REFERENCES orders(id)
        )
    )");
}

void DatabaseManager::executeQuery(const std::string& query){
  char* errMsg = nullptr;
  if(sqlite3_exec(db, query.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK){
    std::string error = errMsg;
    sqlite3_free(errMsg);
    throw std::runtime_error("SQL错误：" + error);
  } 
}

void DatabaseManager::beginTransaction() {
    try {
        if (inTransaction) {
            return;
        }
        executeQuery("BEGIN TRANSACTION;");
        inTransaction = true;
    } catch (const std::exception& e) {
        throw;
    }
}

void DatabaseManager::commit() {
    try {
        if (!inTransaction) {
            return;
        }
        executeQuery("COMMIT;");
        inTransaction = false;
    } catch (const std::exception& e) {
        throw;
    }
}

void DatabaseManager::rollback() {
    try {
        if (!inTransaction) {
            return;
        }
        executeQuery("ROLLBACK;");
        inTransaction = false;
    } catch (const std::exception& e) {
        throw;
    }
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

double DatabaseManager::getUserBalance(const std::string& username) const {
    sqlite3_stmt* stmt = nullptr;
    const char* sql = "SELECT balance FROM users WHERE username = ?";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("准备查询余额语句失败");
    }
    
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    
    double balance = 0.0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        balance = sqlite3_column_double(stmt, 0);
    } else {
        sqlite3_finalize(stmt);
        throw std::runtime_error("用户不存在");
    }
    
    sqlite3_finalize(stmt);
    return balance;
}

bool DatabaseManager::updateBalance(const std::string& username, double newBalance) {
    if (newBalance < 0) {
        throw std::runtime_error("余额不能为负");
    }
    
    sqlite3_stmt* stmt = nullptr;
    const char* sql = "UPDATE users SET balance = ? WHERE username = ?";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("准备更新余额语句失败");
    }
    
    sqlite3_bind_double(stmt, 1, newBalance);
    sqlite3_bind_text(stmt, 2, username.c_str(), -1, SQLITE_STATIC);
    
    bool success = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    
    if (!success) {
        throw std::runtime_error("更新余额失败");
    }
    
    return success;
}

bool DatabaseManager::saveCartItem(const std::string& username, int productId, int quantity) {
    try {
        beginTransaction();
        
        // 检查是否已存在此商品
        sqlite3_stmt* stmt;
        const char* query = "SELECT quantity FROM shopping_cart WHERE username = ? AND product_id = ?";
        
        if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK) {
            throw std::runtime_error("准备查询购物车语句失败");
        }
        
        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 2, productId);
        
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            // 更新现有记录
            sqlite3_finalize(stmt);
            query = "UPDATE shopping_cart SET quantity = ? WHERE username = ? AND product_id = ?";
            
            if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK) {
                throw std::runtime_error("准备更新购物车语句失败");
            }
            
            sqlite3_bind_int(stmt, 1, quantity);
            sqlite3_bind_text(stmt, 2, username.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_int(stmt, 3, productId);
        } else {
            // 插入新记录
            sqlite3_finalize(stmt);
            query = "INSERT INTO shopping_cart (username, product_id, quantity) VALUES (?, ?, ?)";
            
            if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK) {
                throw std::runtime_error("准备插入购物车语句失败");
            }
            
            sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_int(stmt, 2, productId);
            sqlite3_bind_int(stmt, 3, quantity);
        }
        
        bool success = sqlite3_step(stmt) == SQLITE_DONE;
        sqlite3_finalize(stmt);
        
        if (success) {
            commit();
            return true;
        } else {
            rollback();
            return false;
        }
    } catch (const std::exception& e) {
        rollback();
        throw;
    }
}

bool DatabaseManager::removeCartItem(const std::string& username, int productId) {
    try {
        beginTransaction();
        
        const char* query;
        sqlite3_stmt* stmt;
        
        // 如果 productId 为 -1，则清空该用户的整个购物车
        if (productId == -1) {
            query = "DELETE FROM shopping_cart WHERE username = ?";
            
            if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK) {
                throw std::runtime_error("准备清空购物车语句失败");
            }
            
            sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
        } else {
            // 删除单个商品
            query = "DELETE FROM shopping_cart WHERE username = ? AND product_id = ?";
            
            if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK) {
                throw std::runtime_error("准备删除购物车项语句失败");
            }
            
            sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_int(stmt, 2, productId);
        }
        
        bool success = sqlite3_step(stmt) == SQLITE_DONE;
        sqlite3_finalize(stmt);
        
        if (success) {
            commit();
            return true;
        } else {
            rollback();
            return false;
        }
    } catch (const std::exception& e) {
        rollback();
        throw;
    }
}

std::vector<std::pair<int, int>> DatabaseManager::getCartItems(const std::string& username) {
    std::vector<std::pair<int, int>> items;
    const char* query = "SELECT product_id, quantity FROM shopping_cart WHERE username = ?";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("准备查询购物车语句失败");
    }
    
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int productId = sqlite3_column_int(stmt, 0);
        int quantity = sqlite3_column_int(stmt, 1);
        items.emplace_back(productId, quantity);
    }
    
    sqlite3_finalize(stmt);
    return items;
}

int DatabaseManager::createOrder(const std::string& buyerUsername, double totalAmount) {
    const char* query = "INSERT INTO orders (buyer_username, total_amount, status) "
                       "VALUES (?, ?, 'pending')";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("准备创建订单语句失败");
    }
    
    sqlite3_bind_text(stmt, 1, buyerUsername.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 2, totalAmount);
    
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        return -1;
    }
    
    int orderId = sqlite3_last_insert_rowid(db);
    sqlite3_finalize(stmt);
    return orderId;
}

bool DatabaseManager::addOrderItem(int orderId, int productId, int quantity, 
                                double price, const std::string& sellerUsername) {
    const char* query = "INSERT INTO order_items (order_id, product_id, quantity, price, "
                       "seller_username) VALUES (?, ?, ?, ?, ?)";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("准备添加订单项语句失败");
    }
    
    sqlite3_bind_int(stmt, 1, orderId);
    sqlite3_bind_int(stmt, 2, productId);
    sqlite3_bind_int(stmt, 3, quantity);
    sqlite3_bind_double(stmt, 4, price);
    sqlite3_bind_text(stmt, 5, sellerUsername.c_str(), -1, SQLITE_STATIC);
    
    bool success = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    return success;
}

bool DatabaseManager::lockProductStock(int productId, int orderId, int quantity) {
    try {
        beginTransaction();
        
        // 检查库存是否充足
        const char* queryCheck = "SELECT quantity FROM products WHERE id = ?";
        sqlite3_stmt* stmt;
        
        if (sqlite3_prepare_v2(db, queryCheck, -1, &stmt, nullptr) != SQLITE_OK) {
            throw std::runtime_error("准备查询库存语句失败");
        }
        
        sqlite3_bind_int(stmt, 1, productId);
        
        if (sqlite3_step(stmt) != SQLITE_ROW) {
            sqlite3_finalize(stmt);
            rollback();
            return false;
        }
        
        int availableStock = sqlite3_column_int(stmt, 0);
        sqlite3_finalize(stmt);
        
        if (availableStock < quantity) {
            rollback();
            return false;
        }
        
        // 锁定库存
        const char* queryLock = "INSERT INTO product_locks (product_id, order_id, locked_quantity) "
                               "VALUES (?, ?, ?)";
                               
        if (sqlite3_prepare_v2(db, queryLock, -1, &stmt, nullptr) != SQLITE_OK) {
            throw std::runtime_error("准备锁定库存语句失败");
        }
        
        sqlite3_bind_int(stmt, 1, productId);
        sqlite3_bind_int(stmt, 2, orderId);
        sqlite3_bind_int(stmt, 3, quantity);
        
        bool success = sqlite3_step(stmt) == SQLITE_DONE;
        sqlite3_finalize(stmt);
        
        if (success) {
            commit();
            return true;
        } else {
            rollback();
            return false;
        }
    } catch (const std::exception& e) {
        rollback();
        throw;
    }
}

bool DatabaseManager::unlockProductStock(int productId, int orderId) {
    const char* query = "DELETE FROM product_locks WHERE product_id = ? AND order_id = ?";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("准备解锁库存语句失败");
    }
    
    sqlite3_bind_int(stmt, 1, productId);
    sqlite3_bind_int(stmt, 2, orderId);
    
    bool success = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    return success;
}

bool DatabaseManager::updateOrderStatus(int orderId, const std::string& status) {
    const char* query = "UPDATE orders SET status = ? WHERE id = ?";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("准备更新订单状态语句失败");
    }
    
    sqlite3_bind_text(stmt, 1, status.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, orderId);
    
    bool success = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    return success;
}
