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
    
    // 获取指定商家的所有商品
    std::vector<std::shared_ptr<Product>> getSellerProducts(const std::string& sellerUsername) const;
    
    // 更新商品信息
    bool updateProduct(int productId, const std::string& name, const std::string& description, 
                      double price, int quantity);
    
    // 更新商品数量
    bool updateQuantity(int productId, int newQuantity);
    
    // 更新商品价格
    bool updatePrice(int productId, double newPrice);
    
    // 删除商品
    bool deleteProduct(int productId);
    
    // 删除商品
    bool removeProduct(int productId, const std::string& sellerUsername);
    
    // 修改商品价格
    bool updatePrice(int productId, double newPrice, const std::string& sellerUsername);
    
    // 修改商品库存
    bool updateQuantity(int productId, int newQuantity, const std::string& sellerUsername);
    
    // 更新商品基本信息
    bool updateProductInfo(int productId, 
                         const std::string& name,
                         const std::string& description,
                         const std::string& sellerUsername);
    
    // 设置商品折扣
    bool setProductDiscount(int productId, double discountRate, 
                          const std::string& sellerUsername);
    
    // 设置品类折扣
    bool setCategoryDiscount(const std::string& category, double discountRate, 
                           const std::string& sellerUsername);
};

#endif // PRODUCT_MANAGER_H