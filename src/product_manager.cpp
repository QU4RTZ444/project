#include "include.h"
#include "product_manager.h"

void ProductManager::setCategoryDiscount(const std::string& category, double discount) {
  if (discount >= 0.0 && discount <= 1.0) {
      categoryDiscounts[category] = discount;
  }
}