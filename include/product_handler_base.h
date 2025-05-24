#ifndef PRODUCT_HANDLER_BASE_H
#define PRODUCT_HANDLER_BASE_H

#include "menu_handler_base.h"
#include "cart_handler.h"

class ProductHandler : public MenuHandlerBase {
private:
    CartHandler& cartHandler;

public:
    ProductHandler(UserManager& um, ProductManager& pm, CartHandler& ch)
        : MenuHandlerBase(um, pm), cartHandler(ch) {}
        
    void handleManageProducts(const std::string& sellerUsername);
    void handleBrowseProducts(const std::shared_ptr<User>& currentUser);
    void handleSearchProducts(const std::shared_ptr<User>& currentUser);
    void handleAddProduct(const std::string& sellerUsername);
    
private:
    void displaySearchResults(const std::vector<std::shared_ptr<Product>>& products,
                            const std::string& searchCriteria,
                            const std::string& username);
};

#endif