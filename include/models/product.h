/*
@file product.h
@author QU4RTZ444
@date 2025-04-17 16:43
 */
#ifndef product_H
#define product_H

#include "include.h"

/**
 * @brief 商品抽象基类
 * @details 定义了商品的基本属性和操作，作为所有具体商品类型的基类
 */
class Product {
private:
    /** @brief 商品编号，作为唯一标识 */
    int id;
    /** @brief 商品名称 */
    std::string name;
    /** @brief 商品描述信息 */
    std::string description;
    /** @brief 商品基础价格 */
    double basePrice;
    /** @brief 商品库存数量 */
    int quantity;
    /** @brief 商家用户名 */
    std::string sellerUsername;
    /** @brief 折扣率，默认为1表示无折扣 */
    double discountRate;

public:
    Product(int id, const std::string& name, const std::string& desc, 
            double price, int qty, const std::string& seller)
        : id(id), 
          name(name), 
          description(desc), 
          basePrice(price),
          quantity(qty),
          sellerUsername(seller),
          discountRate(1.0) {}  // 初始化折扣率为1.0（无折扣）

    /**
     * @brief 虚析构函数
     */
    virtual ~Product() = default;

    /**
     * @brief 获取商品基础价格（未打折）
     * @return double 基础价格
     */
    double getBasePrice() const { return basePrice; }

    /**
     * @brief 获取商品实际价格（考虑折扣）
     * @return double 实际价格
     */
    virtual double getPrice() const {
        #ifdef DEBUG
        std::cout << "[DEBUG] getPrice() 被调用\n";
        std::cout << "[DEBUG] basePrice = " << basePrice << "\n";
        std::cout << "[DEBUG] discountRate = " << discountRate << "\n";
        std::cout << "[DEBUG] 计算结果 = " << (basePrice * discountRate) << "\n";
        #endif
        return basePrice * discountRate;
    }

    /**
     * @brief 获取商品类别
     * @return string 商品类别的字符串表示
     */
    virtual std::string getCategory() const = 0;

    /**
     * @brief 显示商品详细信息
     */
    virtual void display() const;

    /**
     * @brief 更新商品价格
     * @param newPrice 新价格
     */
    void updatePrice(double newPrice);

    /**
     * @brief 更新商品库存数量
     * @param newQty 新数量
     */
    void updateQuantity(int newQty);

    /**
     * @brief 获取商品名称
     * @return const string& 商品名称
     */
    const std::string& getName() const;

    /**
     * @brief 获取商家用户名
     * @return const string& 商家用户名
     */
    const std::string& getSeller() const { return sellerUsername; }

    /**
     * @brief 获取商品编号
     * @return int 商品编号
     */
    int getId() const { return id; }

    /**
     * @brief 获取商品描述
     * @return const string& 商品描述
     */
    const std::string& getDescription() const { return description; }

    /**
     * @brief 设置商品折扣率
     * @param rate 折扣率(0-1之间)
     */
    void setDiscountRate(double rate) {
        if (rate > 0 && rate <= 1) {
            discountRate = rate;
        }
    }

    /**
     * @brief 获取商品折扣率
     * @return double 当前折扣率
     */
    double getDiscountRate() const { return discountRate; }

    /**
     * @brief 获取商品库存数量
     * @return int 库存数量
     */
    int getQuantity() const { return quantity; }

    /**
     * @brief 设置商品库存数量
     * @param qty 新的库存数量
     * @throw std::runtime_error 如果数量为负数
     */
    void setQuantity(int qty) {
        if (qty < 0) {
            throw std::runtime_error("库存数量不能为负");
        }
        quantity = qty;
    }

    /**
     * @brief 商品状态检查方法
     * @return bool 如果商品有库存则返回true，否则返回false
     */
    bool isAvailable() const { return quantity > 0; }
};

/**
 * @brief 图书类商品
 * @details 继承自Product基类的图书类型商品
 */
class Book : public Product {
public:
    /**
     * @brief 图书类构造函数
     * @param productId 商品编号
     * @param n 图书名称
     * @param desc 图书描述
     * @param price 图书价格
     * @param qty 库存数量
     * @param seller 商家用户名
     */
    Book(int productId, const std::string& n, const std::string& desc, 
         double price, int qty, const std::string& seller)
        : Product(productId, n, desc, price, qty, seller) {}
    
    /**
     * @brief 获取商品类别
     * @return string 返回"图书"
     */
    std::string getCategory() const override { return "图书"; }
};

/**
 * @brief 食品类商品
 * @details 继承自Product基类的食品类型商品
 */
class Food : public Product {
public:
    /**
     * @brief 食品类构造函数
     * @param productId 商品编号
     * @param n 食品名称
     * @param desc 食品描述
     * @param price 食品价格
     * @param qty 库存数量
     * @param seller 商家用户名
     */
    Food(int productId, const std::string& n, const std::string& desc, 
         double price, int qty, const std::string& seller)
        : Product(productId, n, desc, price, qty, seller) {}
    
    /**
     * @brief 获取商品类别
     * @return string 返回"食品"
     */
    std::string getCategory() const override { return "食品"; }
};

/**
 * @brief 服装类商品
 * @details 继承自Product基类的服装类型商品
 */
class Clothing : public Product {
public:
    /**
     * @brief 服装类构造函数
     * @param productId 商品编号
     * @param n 服装名称
     * @param desc 服装描述
     * @param price 服装价格
     * @param qty 库存数量
     * @param seller 商家用户名
     */
    Clothing(int productId, const std::string& n, const std::string& desc, 
             double price, int qty, const std::string& seller)
        : Product(productId, n, desc, price, qty, seller) {}
    
    /**
     * @brief 获取商品类别
     * @return string 返回"服装"
     */
    std::string getCategory() const override { return "服装"; }
};

#endif // product_H