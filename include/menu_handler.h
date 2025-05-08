#ifndef MENU_HANDLER_H
#define MENU_HANDLER_H

#include "include.h"
#include "user_manager.h"
#include "product_manager.h"

class MenuHandler {
private:
    static const int ITEMS_PER_PAGE = 5;
    UserManager& userManager;
    ProductManager& productManager;

    void handleConsumerChoice(const std::shared_ptr<User>& currentUser);
    void handleSellerChoice(const std::shared_ptr<User>& currentUser);
    
    void handleSalesStatistics(const std::string& sellerUsername);

public:
    void handleManageProducts(const std::string& sellerUsername);
    MenuHandler(UserManager& um, ProductManager& pm) 
        : userManager(um), productManager(pm) {}

    void waitForKey() const;
    std::shared_ptr<User> handleLogin();
    bool handleRegister();
    void handleBrowseProducts() const;
    void handleSearchProducts() const;
    void handleBalance(const std::string& username);
    void handleAddProduct(const std::string& sellerUsername);
    void handleChangePassword(const std::string& username);
    void displayUserMenu(const std::shared_ptr<User>& currentUser);
};

#endif