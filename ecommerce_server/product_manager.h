#ifndef PRODUCT_MANAGER_H
#define PRODUCT_MANAGER_H

#include "product.h"
#include <vector>

#include <memory>
#include <string>
#include <mutex>

// ���ڴ�����Ʒ��Ϣ�Ľṹ�壨�Ƕ�̬��
struct ProductInfo {
    int productId;              // ��ƷID
    std::string name;           // ��Ʒ����
    double originalPrice;       // ��Ʒԭ��
    double currentPrice;        // ��Ʒ�ּۣ������ۿۺ�
    int stock;                  // ��Ʒ���
    std::string merchantName;   // �����̼�
    std::string productType;    // ��Ʒ����
    double discount;            // �ۿۣ�0.0-1.0��1.0��ʾ���ۿۣ�0.8��ʾ8�ۣ�

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
    void saveProductsToFile(); // ������˽�з�����������

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

    // �޸ķ������ͣ�ʹ��ProductInfo�ṹ�����Product����
    std::vector<ProductInfo> getAllProducts() const;
    std::vector<ProductInfo> getProductsByPage(int page, int pageSize) const;
    std::vector<ProductInfo> searchProducts(const std::string& keyword) const;
    std::vector<ProductInfo> getProductsByType(const std::string& type) const;

    // �̼�ר�ò�ѯ
    std::vector<ProductInfo> getProductsByMerchant(const std::string& merchantName) const;
    std::vector<ProductInfo> getMerchantProductsByPage(const std::string& merchantName, int page, int pageSize) const;
    int getMerchantProductCount(const std::string& merchantName) const;

    // ����ָ�������޸Ĳ���
    Product* getProductById(int productId);

    size_t getProductCount() const;
    int getTotalPages(int pageSize) const;
};

#endif