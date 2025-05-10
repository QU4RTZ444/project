#ifndef MENU_HANDLER_H
#define MENU_HANDLER_H

#include "include.h"
#include "user_manager.h"
#include "product_manager.h"

class MenuHandler {
private:
    static const int ITEMS_PER_PAGE = 3;
    UserManager& userManager;
    ProductManager& productManager;

    // 辅助函数：显示搜索结果
    void displaySearchResults(const std::vector<std::shared_ptr<Product>>& products,
                            const std::string& searchCriteria) const;
                            
    // 辅助函数：处理分页导航
    bool handlePageNavigation(int choice, int& currentPage, int totalPages) const;

public:
    bool handleConsumerChoice(const std::shared_ptr<User>& currentUser);
    bool handleSellerChoice(const std::shared_ptr<User>& currentUser);
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