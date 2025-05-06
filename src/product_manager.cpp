#include "include.h"

bool ProductManager::addProduct(const std::string& name, const std::string& category,
                              const std::string& description, double price,
                              int quantity, const std::string& sellerUsername) {
    try {
        const char* query = "INSERT INTO products (name, category, description, price, "
                           "quantity, seller_username) VALUES (?, ?, ?, ?, ?, ?);";
        
        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db->getHandle(), query, -1, &stmt, nullptr) != SQLITE_OK) {
            return false;
        }
        
        // 绑定参数
        sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, category.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, description.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_double(stmt, 4, price);
        sqlite3_bind_int(stmt, 5, quantity);
        sqlite3_bind_text(stmt, 6, sellerUsername.c_str(), -1, SQLITE_STATIC);
        
        int result = sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        
        return result == SQLITE_DONE;
    } catch (const std::exception& e) {
        return false;
    }
}

std::vector<std::shared_ptr<Product>> ProductManager::getAllProducts() const {
    std::vector<std::shared_ptr<Product>> products;
    const char* query = "SELECT id, name, category, description, price, quantity, "
                       "seller_username FROM products;";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db->getHandle(), query, -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            std::string category = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            std::string description = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            double price = sqlite3_column_double(stmt, 4);
            int quantity = sqlite3_column_int(stmt, 5);
            
            // 根据不同类别创建对应的商品对象
            std::shared_ptr<Product> product;
            if (category == "食品") {
                product = std::make_shared<Food>(name, description, price, quantity);
            } else if (category == "服装") {
                product = std::make_shared<Clothing>(name, description, price, quantity);
            } else {
                product = std::make_shared<Product>(name, description, price, quantity);
            }
            
            products.push_back(product);
        }
    }
    sqlite3_finalize(stmt);
    return products;
}

std::vector<std::shared_ptr<Product>> ProductManager::searchProducts(const std::string& keyword) const {
    std::vector<std::shared_ptr<Product>> products;
    const char* query = "SELECT id, name, category, description, price, quantity "
                       "FROM products WHERE name LIKE ? OR description LIKE ?;";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db->getHandle(), query, -1, &stmt, nullptr) == SQLITE_OK) {
        std::string searchPattern = "%" + keyword + "%";
        sqlite3_bind_text(stmt, 1, searchPattern.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, searchPattern.c_str(), -1, SQLITE_STATIC);
        
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            // 创建商品对象的代码与 getAllProducts() 类似
            // ...existing code...
        }
    }
    sqlite3_finalize(stmt);
    return products;
}

bool ProductManager::updateQuantity(int productId, int newQuantity) {
    if (newQuantity < 0) return false;
    
    const char* query = "UPDATE products SET quantity = ? WHERE id = ?;";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db->getHandle(), query, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }
    
    sqlite3_bind_int(stmt, 1, newQuantity);
    sqlite3_bind_int(stmt, 2, productId);
    
    int result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    return result == SQLITE_DONE;
}

bool ProductManager::updatePrice(int productId, double newPrice) {
    if (newPrice < 0) return false;
    
    const char* query = "UPDATE products SET price = ? WHERE id = ?;";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db->getHandle(), query, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }
    
    sqlite3_bind_double(stmt, 1, newPrice);
    sqlite3_bind_int(stmt, 2, productId);
    
    int result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    return result == SQLITE_DONE;
}

bool ProductManager::deleteProduct(int productId) {
    const char* query = "DELETE FROM products WHERE id = ?;";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db->getHandle(), query, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }
    
    sqlite3_bind_int(stmt, 1, productId);
    
    int result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    return result == SQLITE_DONE;
}
