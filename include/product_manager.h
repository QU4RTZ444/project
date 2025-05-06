/*
@file product_manager.h
@author QU4RTZ444
@date 2025-04-17 16:49
 */
#ifndef PRODUCT_MANAGER_H
#define PRODUCT_MANAGER_H

#include "include.h"
#include "database_manager.h"

class ProductManager {
private:
    DatabaseManager* db;

public:
    ProductManager() : db(DatabaseManager::getInstance()) {}
    
    // 添加新商品
    bool addProduct(const std::string& name, const std::string& category,
                   const std::string& description, double price, 
                   int quantity, const std::string& sellerUsername);
    
    // 获取所有商品
    std::vector<std::shared_ptr<Product>> getAllProducts() const;
    
    // 搜索商品
    std::vector<std::shared_ptr<Product>> searchProducts(const std::string& keyword) const;
    
    // 更新商品数量
    bool updateQuantity(int productId, int newQuantity);
    
    // 更新商品价格
    bool updatePrice(int productId, double newPrice);
    
    // 删除商品
    bool deleteProduct(int productId);
};

#endif // PRODUCT_MANAGER_H