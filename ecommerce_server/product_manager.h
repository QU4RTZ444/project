#ifndef PRODUCT_MANAGER_H
#define PRODUCT_MANAGER_H

#include "product.h"
#include <vector>

#include <memory>
#include <string>
#include <mutex>

// 用于传输商品信息的结构体（非多态）
struct ProductInfo {
    int productId;              // 商品ID
    std::string name;           // 商品名称
    double originalPrice;       // 商品原价
    double currentPrice;        // 商品现价（考虑折扣后）
    int stock;                  // 商品库存
    std::string merchantName;   // 出售商家
    std::string productType;    // 商品种类
    double discount;            // 折扣（0.0-1.0，1.0表示无折扣，0.8表示8折）

    ProductInfo(const Product& product)
        : productId(product.getProductId()),
        name(product.getName()),
        originalPrice(product.getOriginalPrice()),
        currentPrice(product.getPrice()),
        stock(product.getStock()),
        merchantName(product.getMerchantName()),
        productType(product.getProductType()),
        discount(product.getDiscount()) {}
};

class ProductManager {
private:
    std::vector<std::unique_ptr<Product>> products;
    std::string filename;
    mutable std::mutex productsMutex;
    int nextProductId;

    void loadProducts();
    void saveProductsToFile(); // 新增的私有方法，不加锁

    std::unique_ptr<Product> createProduct(const std::string& type, int id,
        const std::string& name, double price,
        int stock, const std::string& merchant,
        double discount = 1.0);

public:
    void saveProducts();
    ProductManager(const std::string& filename);
    ~ProductManager();

    bool addProduct(const std::string& type, const std::string& name,
        double price, int stock, const std::string& merchantName,
        double discount = 1.0);

    bool modifyProduct(int productId, double newPrice = -1, int newStock = -1, double newDiscount = -1);

    int setDiscountByType(const std::string& productType, double discount);

    // 修改返回类型：使用ProductInfo结构体代替Product对象
    std::vector<ProductInfo> getAllProducts() const;
    std::vector<ProductInfo> getProductsByPage(int page, int pageSize) const;
    std::vector<ProductInfo> searchProducts(const std::string& keyword) const;
    std::vector<ProductInfo> getProductsByType(const std::string& type) const;

    // 商家专用查询
    std::vector<ProductInfo> getProductsByMerchant(const std::string& merchantName) const;
    std::vector<ProductInfo> getMerchantProductsByPage(const std::string& merchantName, int page, int pageSize) const;
    int getMerchantProductCount(const std::string& merchantName) const;

    // 返回指针用于修改操作
    Product* getProductById(int productId);

    size_t getProductCount() const;
    int getTotalPages(int pageSize) const;
};

#endif