#include "include.h"

double Product::getPrice() const{
  return basePrice;
}

void Product::display() const{
  std::cout << "商品名称: " << name << "\n"
              << "描述: " << description << "\n"
              << "价格: " << getPrice() << "\n"
              << "库存: " << quantity << "\n"
              << "商家: " << (sellerUsername.empty() ? "未知" : sellerUsername) << "\n";
}

void Product::updatePrice(double newPrice){
  if(newPrice > 0){
    basePrice = newPrice;
  }
}

void Product::updateQuantity(int newQty){
  if(newQty >= 0){
    quantity = newQty;
  }
}

const std::string &Product::getName() const{
  return name;
}
