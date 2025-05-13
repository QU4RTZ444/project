/*
@file product_manager.h
@author QU4RTZ444
@date 2025-04-17 16:49
*/
#ifndef PRODUCT_MANAGER_H
#define PRODUCT_MANAGER_H

#include "include.h"
#include "database_manager.h"

/**
 * @brief 商品管理类
 * @details 负责商品的增删改查等管理功能
 */
class ProductManager {
private:
    /** @brief 数据库管理器指针 */
    DatabaseManager* db;

public:
    /**
     * @brief 构造函数
     * @details 初始化数据库连接
     */
    ProductManager() : db(DatabaseManager::getInstance()) {}
    
    /**
     * @brief 添加新商品
     * @param name 商品名称
     * @param category 商品类别
     * @param description 商品描述
     * @param price 商品价格
     * @param quantity 商品数量
     * @param sellerUsername 卖家用户名
     * @return bool 添加成功返回true，失败返回false
     */
    bool addProduct(const std::string& name, const std::string& category,
                   const std::string& description, double price, 
                   int quantity, const std::string& sellerUsername);
    
    /**
     * @brief 获取所有商品
     * @return vector<shared_ptr<Product>> 商品对象列表
     */
    std::vector<std::shared_ptr<Product>> getAllProducts() const;
    
    /**
     * @brief 关键字搜索商品
     * @param keyword 搜索关键词
     * @return vector<shared_ptr<Product>> 匹配的商品列表
     */
    std::vector<std::shared_ptr<Product>> searchProducts(const std::string& keyword) const;
    
    /**
     * @brief 获取指定商家的所有商品
     * @param sellerUsername 商家用户名
     * @return vector<shared_ptr<Product>> 该商家的商品列表
     */
    std::vector<std::shared_ptr<Product>> getSellerProducts(const std::string& sellerUsername) const;
    
    /**
     * @brief 更新商品信息
     * @param productId 商品ID
     * @param name 新商品名称
     * @param description 新商品描述
     * @param price 新价格
     * @param quantity 新数量
     * @return bool 更新成功返回true，失败返回false
     */
    bool updateProduct(int productId, const std::string& name, const std::string& description, 
                      double price, int quantity);
    
    /**
     * @brief 更新商品数量
     * @param productId 商品ID
     * @param newQuantity 新数量
     * @return bool 更新成功返回true，失败返回false
     */
    bool updateQuantity(int productId, int newQuantity);
    
    /**
     * @brief 更新商品价格
     * @param productId 商品ID
     * @param newPrice 新价格
     * @return bool 更新成功返回true，失败返回false
     */
    bool updatePrice(int productId, double newPrice);
    
    /**
     * @brief 删除商品（管理员功能）
     * @param productId 商品ID
     * @return bool 删除成功返回true，失败返回false
     */
    bool deleteProduct(int productId);
    
    /**
     * @brief 商家删除自己的商品
     * @param productId 商品ID
     * @param sellerUsername 商家用户名
     * @return bool 删除成功返回true，失败返回false
     */
    bool removeProduct(int productId, const std::string& sellerUsername);
    
    /**
     * @brief 商家修改商品价格
     * @param productId 商品ID
     * @param newPrice 新价格
     * @param sellerUsername 商家用户名
     * @return bool 修改成功返回true，失败返回false
     */
    bool updatePrice(int productId, double newPrice, const std::string& sellerUsername);
    
    /**
     * @brief 商家修改商品库存
     * @param productId 商品ID
     * @param newQuantity 新库存数量
     * @param sellerUsername 商家用户名
     * @return bool 修改成功返回true，失败返回false
     */
    bool updateQuantity(int productId, int newQuantity, const std::string& sellerUsername);
    
    /**
     * @brief 更新商品基本信息
     * @param productId 商品ID
     * @param name 新名称
     * @param description 新描述
     * @param sellerUsername 商家用户名
     * @return bool 更新成功返回true，失败返回false
     */
    bool updateProductInfo(int productId, 
                         const std::string& name,
                         const std::string& description,
                         const std::string& sellerUsername);
    
    /**
     * @brief 设置商品折扣
     * @param productId 商品ID
     * @param discountRate 折扣率(0-1之间)
     * @param sellerUsername 商家用户名
     * @return bool 设置成功返回true，失败返回false
     */
    bool setProductDiscount(int productId, double discountRate, 
                          const std::string& sellerUsername);
    
    /**
     * @brief 设置品类折扣
     * @param category 商品类别
     * @param discountRate 折扣率(0-1之间)
     * @param sellerUsername 商家用户名
     * @return bool 设置成功返回true，失败返回false
     */
    bool setCategoryDiscount(const std::string& category, double discountRate, 
                           const std::string& sellerUsername);
    
    /**
     * @brief 按类别搜索商品
     * @param category 商品类别
     * @return vector<shared_ptr<Product>> 匹配的商品列表
     */
    std::vector<std::shared_ptr<Product>> searchByCategory(const std::string& category) const;
    
    /**
     * @brief 按价格范围搜索商品
     * @param minPrice 最低价格
     * @param maxPrice 最高价格
     * @return vector<shared_ptr<Product>> 匹配的商品列表
     */
    std::vector<std::shared_ptr<Product>> searchByPriceRange(double minPrice, double maxPrice) const;
    
    /**
     * @brief 按名称搜索商品
     * @param keyword 商品名称关键词
     * @return vector<shared_ptr<Product>> 匹配的商品列表
     */
    std::vector<std::shared_ptr<Product>> searchByName(const std::string& keyword) const;
};

#endif // PRODUCT_MANAGER_H