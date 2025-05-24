#include "include.h"

void Product::display() const {
    std::cout << "商品ID: " << id << "\n"
              << "名称: " << name << "\n"
              << "类别: " << getCategory() << "\n"
              << "描述: " << description << "\n";
    
    std::cout << std::fixed << std::setprecision(2);
    DEBUG_LOG("折扣率: " << discountRate);
    DEBUG_LOG("基础价格: " << basePrice);
    DEBUG_LOG("计算后价格: " << getPrice());

    if (discountRate < 1.0) {
        std::cout << "原价: ￥" << basePrice << "\n"
                  << "折扣: " << (discountRate * 10) << "折\n"
                  << "折后价: ￥" << getPrice() << "\n";
    } else {
        std::cout << "价格: ￥" << getPrice() << "\n";
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
