/*
@file product_manager.h
@author QU4RTZ444
@date 2025-04-17 16:49
 */
#ifndef product_manager_H
#define product_manager_H

#include "include.h"

class ProductManager {
  private:
      std::vector<std::shared_ptr<Product>> products;
  
  public:
      void addProduct(const std::shared_ptr<Product>& product);
      void showAllProducts() const;
      void searchProduct(const std::string& keyword) const;
      void saveToFile() const;
      void loadFromFile();
};

#endif // product_manager_H