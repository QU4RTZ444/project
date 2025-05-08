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
    int id;                  // 商品编号，作为唯一标识
    std::string name;        // 商品名称
    std::string description; // 商品描述信息
    double basePrice;        // 商品基础价格
    int quantity;            // 商品库存数量
    std::string sellerUsername; // 商家用户名

public:
    // 修改构造函数，添加 id 参数
    Product(int productId, const std::string& n, const std::string& desc, 
            double price, int qty, const std::string& seller = "")
        : id(productId), name(n), description(desc), basePrice(price), 
          quantity(qty), sellerUsername(seller) {}

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

    // 获取商品编号
    int getId() const { return id; }
};

// 修改子类的构造函数
class Book : public Product {
public:
    Book(int productId, const std::string& n, const std::string& desc, 
         double price, int qty, const std::string& seller = "")
        : Product(productId, n, desc, price, qty, seller) {}
    std::string getCategory() const override { return "图书"; }
};

class Food : public Product {
public:
    Food(int productId, const std::string& n, const std::string& desc, 
         double price, int qty, const std::string& seller = "")
        : Product(productId, n, desc, price, qty, seller) {}
    std::string getCategory() const override { return "食品"; }
};

class Clothing : public Product {
public:
    Clothing(int productId, const std::string& n, const std::string& desc, 
             double price, int qty, const std::string& seller = "")
        : Product(productId, n, desc, price, qty, seller) {}
    std::string getCategory() const override { return "服装"; }
};

#endif // product_H