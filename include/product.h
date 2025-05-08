/*
@file product.h
@author QU4RTZ444
@date 2025-04-17 16:43
 */
#ifndef product_H
#define product_H

#include "include.h"

// ====================== 商品抽象类 ========================
class Product {
private:
    std::string name;         // 商品名称，作为商品的标识
    std::string description;  // 商品描述信息
    double basePrice;         // 商品基础价格，未计算折扣
    int quantity;             // 商品库存数量
    std::string sellerUsername; // 商家用户名

public:
    // 构造函数：初始化商品基本信息
    // @param n: 商品名称
    // @param desc: 商品描述
    // @param price: 商品价格
    // @param qty: 商品数量
    Product(const std::string& n, const std::string& desc, double price, int qty, const std::string& seller = "")
        : name(n), description(desc), basePrice(price), quantity(qty), sellerUsername(seller) {}

    // 虚析构函数
    virtual ~Product() = default;

    // 获取商品实际价格（考虑折扣等因素）
    virtual double getPrice() const;

    // 纯虚函数：获取商品类别
    // @return: 返回商品类别的字符串表示
    virtual std::string getCategory() const = 0;

    virtual void display() const;

    void updatePrice(double newPrice);

    void updateQuantity(int newQty);

    const std::string& getName() const;

    const std::string& getSeller() const { return sellerUsername; }
};

class Book : public Product {
public:
    Book(const std::string& n, const std::string& desc, double price, int qty,
         const std::string& seller = "")
        : Product(n, desc, price, qty, seller) {}
    std::string getCategory() const override { return "图书"; }
};

class Food : public Product {
public:
    Food(const std::string& n, const std::string& desc, double price, int qty,
         const std::string& seller = "")
        : Product(n, desc, price, qty, seller) {}
    std::string getCategory() const override { return "食品"; }
};

class Clothing : public Product {
public:
    Clothing(const std::string& n, const std::string& desc, double price, int qty,
             const std::string& seller = "")
        : Product(n, desc, price, qty, seller) {}
    std::string getCategory() const override { return "服装"; }
};

#endif // product_H