#include "include.h"

double Product::getPrice() const{
  return basePrice * discountRate;
}

void Product::display() const {
    std::cout << "商品ID: " << id << "\n"
              << "名称: " << name << "\n"
              << "类别: " << getCategory() << "\n"
              << "描述: " << description << "\n";
    
    // 显示价格信息
    if (discountRate < 1.0) {
        // 有折扣时显示原价、折扣和折后价
        std::cout << "原价: " << basePrice << "元\n"
                  << "折扣: " << (discountRate * 10) << "折\n"
                  << "折后价: " << getPrice() << "元\n";
    } else {
        // 无折扣时只显示价格
        std::cout << "价格: " << getPrice() << "元\n";
    }
    
    std::cout << "库存: " << quantity << "\n"
              << "卖家: " << sellerUsername << "\n";
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
