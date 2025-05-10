#include "include.h"

bool ProductManager::addProduct(const std::string& name, const std::string& category,
                              const std::string& description, double price,
                              int quantity, const std::string& sellerUsername) {
    try {
        // 添加参数验证
        if (name.empty() || category.empty() || price <= 0 || quantity < 0) {
            return false;
        }

        // 首先插入记录并获取新的商品ID
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
        
        if (result != SQLITE_DONE) {
            return false;
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "添加商品异常: " << e.what() << std::endl;
        return false;
    }
}

std::vector<std::shared_ptr<Product>> ProductManager::getAllProducts() const {
    std::vector<std::shared_ptr<Product>> products;
    const char* query = "SELECT id, name, category, description, price, quantity, "
                       "seller_username, discount_rate FROM products;";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db->getHandle(), query, -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            std::string category = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            std::string description = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            double price = sqlite3_column_double(stmt, 4);
            int quantity = sqlite3_column_int(stmt, 5);
            std::string seller = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
            double discount = sqlite3_column_double(stmt, 7);
            
            std::shared_ptr<Product> product;
            if (category == "图书") {
                product = std::make_shared<Book>(id, name, description, price, quantity, seller);
            } else if (category == "食品") {
                product = std::make_shared<Food>(id, name, description, price, quantity, seller);
            } else if (category == "服装") {
                product = std::make_shared<Clothing>(id, name, description, price, quantity, seller);
            }
            
            if (product) {
                product->setDiscountRate(discount);  // 设置折扣率
                products.push_back(product);
            }
        }
    }
    sqlite3_finalize(stmt);
    return products;
}

std::vector<std::shared_ptr<Product>> ProductManager::searchProducts(const std::string& keyword) const {
    std::vector<std::shared_ptr<Product>> products;
    const char* query = "SELECT id, name, category, description, price, quantity, "
                       "seller_username, discount_rate "  // 添加 discount_rate 字段
                       "FROM products WHERE name LIKE ? OR description LIKE ?;";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db->getHandle(), query, -1, &stmt, nullptr) == SQLITE_OK) {
        std::string searchPattern = "%" + keyword + "%";
        sqlite3_bind_text(stmt, 1, searchPattern.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, searchPattern.c_str(), -1, SQLITE_STATIC);
        
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            std::string category = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            std::string description = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            double price = sqlite3_column_double(stmt, 4);
            int quantity = sqlite3_column_int(stmt, 5);
            std::string seller = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
            double discount = sqlite3_column_double(stmt, 7);  // 获取折扣率
            
            std::shared_ptr<Product> product;
            if (category == "图书") {
                product = std::make_shared<Book>(id, name, description, price, quantity, seller);
            } else if (category == "食品") {
                product = std::make_shared<Food>(id, name, description, price, quantity, seller);
            } else if (category == "服装") {
                product = std::make_shared<Clothing>(id, name, description, price, quantity, seller);
            }
            
            if (product) {
                product->setDiscountRate(discount);  // 设置折扣率
                products.push_back(product);
            }
        }
    }
    sqlite3_finalize(stmt);
    return products;
}

std::vector<std::shared_ptr<Product>> ProductManager::getSellerProducts(const std::string& sellerUsername) const {
    std::vector<std::shared_ptr<Product>> products;
    const char* query = "SELECT id, name, category, description, price, quantity, "
                       "seller_username, discount_rate FROM products WHERE seller_username = ?;";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db->getHandle(), query, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, sellerUsername.c_str(), -1, SQLITE_STATIC);
        
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            const char* namePtr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            const char* categoryPtr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            const char* descPtr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            
            std::string name = namePtr ? namePtr : "";
            std::string category = categoryPtr ? categoryPtr : "";
            std::string description = descPtr ? descPtr : "";
            
            double price = sqlite3_column_double(stmt, 4);
            int quantity = sqlite3_column_int(stmt, 5);
            double discount = sqlite3_column_double(stmt, 7);
            
            try {
                std::shared_ptr<Product> product;
                if (category == "图书") {
                    product = std::make_shared<Book>(id, name, description, price, quantity, sellerUsername);
                } else if (category == "食品") {
                    product = std::make_shared<Food>(id, name, description, price, quantity, sellerUsername);
                } else if (category == "服装") {
                    product = std::make_shared<Clothing>(id, name, description, price, quantity, sellerUsername);
                }
                
                if (product) {
                    product->setDiscountRate(discount);  // 设置折扣率
                    products.push_back(product);
                }
            } catch (const std::exception& e) {
                std::cerr << "创建商品对象失败: " << e.what() << std::endl;
                continue;
            }
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

bool ProductManager::updatePrice(int productId, double newPrice, const std::string& sellerUsername) {
    if (newPrice <= 0) return false;
    
    const char* query = "UPDATE products SET price = ? "
                       "WHERE id = ? AND seller_username = ?;";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db->getHandle(), query, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }
    
    sqlite3_bind_double(stmt, 1, newPrice);
    sqlite3_bind_int(stmt, 2, productId);
    sqlite3_bind_text(stmt, 3, sellerUsername.c_str(), -1, SQLITE_STATIC);
    
    int result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    return result == SQLITE_DONE;
}

bool ProductManager::updateQuantity(int productId, int newQuantity, const std::string& sellerUsername) {
    if (newQuantity < 0) return false;
    
    const char* query = "UPDATE products SET quantity = ? "
                       "WHERE id = ? AND seller_username = ?;";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db->getHandle(), query, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }
    
    sqlite3_bind_int(stmt, 1, newQuantity);
    sqlite3_bind_int(stmt, 2, productId);
    sqlite3_bind_text(stmt, 3, sellerUsername.c_str(), -1, SQLITE_STATIC);
    
    int result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    return result == SQLITE_DONE;
}

bool ProductManager::removeProduct(int productId, const std::string& sellerUsername) {
    const char* query = "DELETE FROM products WHERE id = ? AND seller_username = ?;";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db->getHandle(), query, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }
    
    sqlite3_bind_int(stmt, 1, productId);
    sqlite3_bind_text(stmt, 2, sellerUsername.c_str(), -1, SQLITE_STATIC);
    
    int result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    return result == SQLITE_DONE;
}

bool ProductManager::updateProduct(int productId, const std::string& name, 
                                 const std::string& description, double price, int quantity) {
    const char* query = "UPDATE products SET name = ?, description = ?, "
                       "price = ?, quantity = ? WHERE id = ?;";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db->getHandle(), query, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }
    
    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, description.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 3, price);
    sqlite3_bind_int(stmt, 4, quantity);
    sqlite3_bind_int(stmt, 5, productId);
    
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

bool ProductManager::updateProductInfo(int productId, 
                                     const std::string& name,
                                     const std::string& description,
                                     const std::string& sellerUsername) {
    if (name.empty()) return false;
    
    const char* query = "UPDATE products SET name = ?, description = ? "
                       "WHERE id = ? AND seller_username = ?;";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db->getHandle(), query, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }
    
    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, description.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, productId);
    sqlite3_bind_text(stmt, 4, sellerUsername.c_str(), -1, SQLITE_STATIC);
    
    int result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    return result == SQLITE_DONE;
}

bool ProductManager::setProductDiscount(int productId, double discountRate,
                                      const std::string& sellerUsername) {
    #ifdef DEBUG
    std::cout << "开始设置商品折扣...\n";
    std::cout << "商品ID: " << productId << "\n";
    std::cout << "折扣率: " << discountRate << "\n";
    std::cout << "商家: " << sellerUsername << "\n";
    #endif

    if (discountRate <= 0 || discountRate > 1) {
        #ifdef DEBUG
        std::cout << "折扣率无效: " << discountRate << "\n";
        #endif
        return false;
    }
    
    const char* query = "UPDATE products SET discount_rate = ? "
                       "WHERE id = ? AND seller_username = ?;";
    
    #ifdef DEBUG
    std::cout << "准备执行SQL查询: " << query << "\n";
    #endif

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db->getHandle(), query, -1, &stmt, nullptr) != SQLITE_OK) {
        #ifdef DEBUG
        std::cout << "SQL准备失败: " << sqlite3_errmsg(db->getHandle()) << "\n";
        #endif
        return false;
    }
    
    #ifdef DEBUG
    std::cout << "绑定参数...\n";
    #endif

    sqlite3_bind_double(stmt, 1, discountRate);
    sqlite3_bind_int(stmt, 2, productId);
    sqlite3_bind_text(stmt, 3, sellerUsername.c_str(), -1, SQLITE_STATIC);
    
    int result = sqlite3_step(stmt);
    #ifdef DEBUG
    std::cout << "SQL执行结果: " << result << "\n";
    std::cout << "SQLITE_DONE = " << SQLITE_DONE << "\n";
    if (result != SQLITE_DONE) {
        std::cout << "错误信息: " << sqlite3_errmsg(db->getHandle()) << "\n";
    }
    #endif

    sqlite3_finalize(stmt);
    
    #ifdef DEBUG
    std::cout << "设置商品折扣" << (result == SQLITE_DONE ? "成功" : "失败") << "\n";
    #endif

    return result == SQLITE_DONE;
}

bool ProductManager::setCategoryDiscount(const std::string& category, double discountRate,
                                       const std::string& sellerUsername) {
    #ifdef DEBUG
    std::cout << "开始设置品类折扣...\n";
    std::cout << "品类: " << category << "\n";
    std::cout << "折扣率: " << discountRate << "\n";
    std::cout << "商家: " << sellerUsername << "\n";
    #endif

    if (discountRate <= 0 || discountRate > 1) {
        #ifdef DEBUG
        std::cout << "折扣率无效: " << discountRate << "\n";
        #endif
        return false;
    }
    
    const char* query = "UPDATE products SET discount_rate = ? "
                       "WHERE category = ? AND seller_username = ?;";
    
    #ifdef DEBUG
    std::cout << "准备执行SQL查询: " << query << "\n";
    #endif

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db->getHandle(), query, -1, &stmt, nullptr) != SQLITE_OK) {
        #ifdef DEBUG
        std::cout << "SQL准备失败: " << sqlite3_errmsg(db->getHandle()) << "\n";
        #endif
        return false;
    }
    
    #ifdef DEBUG
    std::cout << "绑定参数...\n";
    #endif

    sqlite3_bind_double(stmt, 1, discountRate);
    sqlite3_bind_text(stmt, 2, category.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, sellerUsername.c_str(), -1, SQLITE_STATIC);
    
    int result = sqlite3_step(stmt);
    #ifdef DEBUG
    std::cout << "SQL执行结果: " << result << "\n";
    std::cout << "SQLITE_DONE = " << SQLITE_DONE << "\n";
    if (result != SQLITE_DONE) {
        std::cout << "错误信息: " << sqlite3_errmsg(db->getHandle()) << "\n";
    }
    #endif

    sqlite3_finalize(stmt);
    
    #ifdef DEBUG
    std::cout << "设置品类折扣" << (result == SQLITE_DONE ? "成功" : "失败") << "\n";
    #endif

    return result == SQLITE_DONE;
}

std::vector<std::shared_ptr<Product>> ProductManager::searchByCategory(const std::string& category) const {
    std::vector<std::shared_ptr<Product>> products;
    const char* query = "SELECT id, name, category, description, price, quantity, "
                       "seller_username, discount_rate FROM products WHERE category = ?;";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db->getHandle(), query, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, category.c_str(), -1, SQLITE_STATIC);
        
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            std::string desc = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            double price = sqlite3_column_double(stmt, 4);
            int quantity = sqlite3_column_int(stmt, 5);
            std::string seller = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
            double discount = sqlite3_column_double(stmt, 7);
            
            std::shared_ptr<Product> product;
            if (category == "图书") {
                product = std::make_shared<Book>(id, name, desc, price, quantity, seller);
            } else if (category == "食品") {
                product = std::make_shared<Food>(id, name, desc, price, quantity, seller);
            } else if (category == "服装") {
                product = std::make_shared<Clothing>(id, name, desc, price, quantity, seller);
            }
            
            if (product) {
                product->setDiscountRate(discount);  // 设置折扣率
                products.push_back(product);
            }
        }
    }
    sqlite3_finalize(stmt);
    return products;
}

std::vector<std::shared_ptr<Product>> ProductManager::searchByPriceRange(
    double minPrice, double maxPrice) const {
    std::vector<std::shared_ptr<Product>> products;
    const char* query = "SELECT id, name, category, description, price, quantity, "
                       "seller_username, discount_rate FROM products WHERE price BETWEEN ? AND ?;";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db->getHandle(), query, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_double(stmt, 1, minPrice);
        sqlite3_bind_double(stmt, 2, maxPrice);
        
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            std::string category = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            std::string desc = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            double price = sqlite3_column_double(stmt, 4);
            int quantity = sqlite3_column_int(stmt, 5);
            std::string seller = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
            double discount = sqlite3_column_double(stmt, 7);
            
            std::shared_ptr<Product> product;
            if (category == "图书") {
                product = std::make_shared<Book>(id, name, desc, price, quantity, seller);
            } else if (category == "食品") {
                product = std::make_shared<Food>(id, name, desc, price, quantity, seller);
            } else if (category == "服装") {
                product = std::make_shared<Clothing>(id, name, desc, price, quantity, seller);
            }
            
            if (product) {
                product->setDiscountRate(discount);  // 设置折扣率
                products.push_back(product);
            }
        }
    }
    sqlite3_finalize(stmt);
    return products;
}

std::vector<std::shared_ptr<Product>> ProductManager::searchByName(const std::string& keyword) const {
    std::vector<std::shared_ptr<Product>> products;
    const char* query = "SELECT id, name, category, description, price, quantity, "
                       "seller_username, discount_rate FROM products WHERE name LIKE ?;";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db->getHandle(), query, -1, &stmt, nullptr) == SQLITE_OK) {
        std::string searchPattern = "%" + keyword + "%";
        sqlite3_bind_text(stmt, 1, searchPattern.c_str(), -1, SQLITE_STATIC);
        
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            std::string category = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            std::string desc = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            double price = sqlite3_column_double(stmt, 4);
            int quantity = sqlite3_column_int(stmt, 5);
            std::string seller = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
            double discount = sqlite3_column_double(stmt, 7);
            
            std::shared_ptr<Product> product;
            if (category == "图书") {
                product = std::make_shared<Book>(id, name, desc, price, quantity, seller);
            } else if (category == "食品") {
                product = std::make_shared<Food>(id, name, desc, price, quantity, seller);
            } else if (category == "服装") {
                product = std::make_shared<Clothing>(id, name, desc, price, quantity, seller);
            }
            
            if (product) {
                product->setDiscountRate(discount);  // 设置折扣率
                products.push_back(product);
            }
        }
    }
    sqlite3_finalize(stmt);
    return products;
}
