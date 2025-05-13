#ifndef MENU_HANDLER_H
#define MENU_HANDLER_H

#include "include.h"
#include "user_manager.h"
#include "product_manager.h"

/**
 * @brief 菜单处理类
 * @details 负责处理所有菜单相关的交互操作，包括用户操作和商品管理
 */
class MenuHandler {
private:
    /** @brief 每页显示的商品数量 */
    static const int ITEMS_PER_PAGE = 3;
    
    /** @brief 用户管理器引用 */
    UserManager& userManager;
    
    /** @brief 商品管理器引用 */
    ProductManager& productManager;

    /**
     * @brief 显示搜索结果
     * @param products 商品列表
     * @param searchCriteria 搜索条件描述
     */
    void displaySearchResults(const std::vector<std::shared_ptr<Product>>& products,
                            const std::string& searchCriteria) const;
                            
    /**
     * @brief 处理分页导航
     * @param choice 用户选择
     * @param currentPage 当前页码
     * @param totalPages 总页数
     * @return bool 如果用户选择退出返回true，否则返回false
     */
    bool handlePageNavigation(int choice, int& currentPage, int totalPages) const;

public:
    /**
     * @brief 处理消费者的菜单选择
     * @param currentUser 当前登录的用户
     * @return bool 如果用户选择退出返回true，否则返回false
     */
    bool handleConsumerChoice(const std::shared_ptr<User>& currentUser);
    
    /**
     * @brief 处理商家的菜单选择
     * @param currentUser 当前登录的用户
     * @return bool 如果用户选择退出返回true，否则返回false
     */
    bool handleSellerChoice(const std::shared_ptr<User>& currentUser);
    
    /**
     * @brief 处理商品管理相关操作
     * @param sellerUsername 商家用户名
     */
    void handleManageProducts(const std::string& sellerUsername);
    
    /**
     * @brief 构造函数
     * @param um 用户管理器
     * @param pm 商品管理器
     */
    MenuHandler(UserManager& um, ProductManager& pm) 
        : userManager(um), productManager(pm) {}

    /**
     * @brief 等待用户按键继续
     */
    void waitForKey() const;
    
    /**
     * @brief 处理用户登录
     * @return shared_ptr<User> 登录成功返回用户对象，失败返回nullptr
     */
    std::shared_ptr<User> handleLogin();
    
    /**
     * @brief 处理用户注册
     * @return bool 注册成功返回true，失败返回false
     */
    bool handleRegister();
    
    /**
     * @brief 处理商品浏览功能
     */
    void handleBrowseProducts() const;
    
    /**
     * @brief 处理商品搜索功能
     */
    void handleSearchProducts() const;
    
    /**
     * @brief 处理账户余额管理
     * @param username 用户名
     */
    void handleBalance(const std::string& username);
    
    /**
     * @brief 处理添加商品功能
     * @param sellerUsername 商家用户名
     */
    void handleAddProduct(const std::string& sellerUsername);
    
    /**
     * @brief 处理密码修改功能
     * @param username 用户名
     */
    void handleChangePassword(const std::string& username);
    
    /**
     * @brief 显示用户菜单
     * @param currentUser 当前登录的用户
     */
    void displayUserMenu(const std::shared_ptr<User>& currentUser);
};

#endif