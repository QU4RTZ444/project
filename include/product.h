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
      std::string name;
      std::string description;
      double basePrice;
      int quantity;
  
  public:
      Product(const std::string& n, const std::string& desc, double price, int qty)
          : name(n), description(desc), basePrice(price), quantity(qty) {}
  
      virtual ~Product() = default;
      virtual double getPrice() const;
      virtual std::string getCategory() const = 0;
      virtual void display() const;
  
      void updatePrice(double newPrice);
      void updateQuantity(int newQty);
      const std::string& getName() const;
};

class Book : public Product {
  public:
      Book(const std::string& n, const std::string& desc, double price, int qty)
          : Product(n, desc, price, qty) {}
      std::string getCategory() const override;
};

class Food : public Product {
  public:
      Food(const std::string& n, const std::string& desc, double price, int qty)
          : Product(n, desc, price, qty) {}
      std::string getCategory();
};

class Clothing : public Product {
  public:
    Clothing(const std::string& n, const std::string& desc, double price, int qty)
      : Product(n, desc, price, qty) {}
    std::string getCategory();
};

#endif // product_H