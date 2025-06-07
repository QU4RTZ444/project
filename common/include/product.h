#ifndef PRODUCT_H
#define PRODUCT_H

#include <string>
#include <fstream>
#include <vector>

/**
 * @brief 商品基类（抽象类）
 * 包含商品的基本信息：名称、价格、库存、商家、折扣
 */
class Product {
protected:
    int productId;
    std::string name;
    double price;              // 原价
    int stock;
    std::string merchantName;
    double discount;           // 折扣
    int frozenStock;           // 冻结库存（为订单管理预留）

public:
    /**
     * @brief 商品基类构造函数
     * @param id 商品ID
     * @param name 商品名称
     * @param price 商品价格
     * @param stock 商品库存
     * @param merchant 出售商家
     * @param discount 折扣（默认1.0无折扣）
     */
    Product(int id, const std::string& name, double price, int stock,
        const std::string& merchant, double discount = 1.0);

    virtual ~Product() = default;

    // Getter方法
    int getProductId() const { return productId; }
    std::string getName() const { return name; }
    double getOriginalPrice() const { return price; }  // 获取原价
    virtual double getPrice() const;                   // 获取现价（考虑折扣）
    int getStock() const { return stock; }
    std::string getMerchantName() const { return merchantName; }
    double getDiscount() const { return discount; }
    int getFrozenStock() const { return frozenStock; } // 获取冻结库存
    virtual std::string getProductType() const = 0;

    // Setter方法
    void setPrice(double newPrice) { price = newPrice; }
    void setStock(int newStock) { stock = newStock; }
    void setDiscount(double newDiscount);

    // 库存管理
    bool reduceStock(int quantity);
    bool increaseStock(int quantity);

    // 冻结库存管理（为订单管理预留）
    bool freezeStock(int quantity);
    bool unfreezeStock(int quantity);

    // 序列化方法
    virtual void serialize(std::ofstream& out) const;
    virtual void deserialize(std::ifstream& in);

    // 格式化显示
    std::string toString() const;
    std::string toDetailString() const;

    // 为交易功能预留：检查是否可购买指定数量
    bool isAvailable(int quantity) const;

    // 检查是否有折扣
    bool hasDiscount() const { return discount < 1.0; }
};

/**
 * @brief 食品类
 * 继承自Product基类
 */
class Food : public Product {
public:
    /**
     * @brief 食品类构造函数
     */
    Food(int id, const std::string& name, double price, int stock,
        const std::string& merchant, double discount = 1.0);

    /**
     * @brief 实现纯虚函数 - 返回商品类型
     * @return "食品"
     */
    std::string getProductType() const override { return "食品"; }

    /**
     * @brief 重写价格计算（食品可能有特殊折扣规则）
     * @return 实际价格
     */
    double getPrice() const override;
};

/**
 * @brief 书籍类
 * 继承自Product基类
 */
class Book : public Product {
public:
    /**
     * @brief 书籍类构造函数
     */
    Book(int id, const std::string& name, double price, int stock,
        const std::string& merchant, double discount = 1.0);

    /**
     * @brief 实现纯虚函数 - 返回商品类型
     * @return "书籍"
     */
    std::string getProductType() const override { return "书籍"; }

    /**
     * @brief 重写价格计算（书籍可能有教育折扣等）
     * @return 实际价格
     */
    double getPrice() const override;
};

/**
 * @brief 衣服类
 * 继承自Product基类
 */
class Clothing : public Product {
public:
    /**
     * @brief 衣服类构造函数
     */
    Clothing(int id, const std::string& name, double price, int stock,
        const std::string& merchant, double discount = 1.0);

    /**
     * @brief 实现纯虚函数 - 返回商品类型
     * @return "衣服"
     */
    std::string getProductType() const override { return "衣服"; }

    /**
     * @brief 重写价格计算（衣服可能有季节性折扣等）
     * @return 实际价格
     */
    double getPrice() const override;
};

#endif