#include "include.h"

double Product::getPrice() const{
  return basePrice * discountRate;
}

void Product::display() const {
    try {
        std::cout << "商品ID: " << id << "\n"
                  << "名称: " << name << "\n"
                  << "类别: " << getCategory() << "\n"
                  << "描述: " << description << "\n"
                  << "价格: " << basePrice << "元\n"
                  << "库存: " << quantity << "\n"
                  << "卖家: " << sellerUsername << "\n";
    } catch (const std::exception& e) {
        throw std::runtime_error("显示商品信息时出错: " + std::string(e.what()));
    }
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
