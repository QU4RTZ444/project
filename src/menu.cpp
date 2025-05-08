#include "include.h"
#include "menu.h"
#include "platform_utils.h"

namespace {
    const int ITEMS_PER_PAGE = 5;  // 每页显示的商品数量

    // 等待用户按键继续
    void waitForKey() {
        std::cout << "\n按回车键继续...";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cin.get();
    }

    // 处理用户登录功能
    std::shared_ptr<User> handleLogin(UserManager& userManager) {
        std::string username, password;
        std::cout << "\n=== 用户登录 ===\n";
        std::cout << "请输入用户名: ";
        std::cin >> username;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  // 清除输入缓冲区
        
        std::cout << "请输入密码: ";
        password = PlatformUtils::getHiddenInput();
        
        auto currentUser = userManager.login(username, password);
        if (currentUser) {
            std::cout << "登录成功！欢迎回来，" << username << "！\n";
        } else {
            std::cout << "登录失败：用户名或密码错误\n";
        }
        return currentUser;
    }

    // 处理用户注册功能
    bool handleRegister(UserManager& userManager) {
        std::string username, password, confirm_password, type;
        std::cout << "\n=== 用户注册 ===\n";
        std::cout << "请输入用户名: ";
        std::cin >> username;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        
        std::cout << "请输入密码: ";
        password = PlatformUtils::getHiddenInput();
        
        std::cout << "请确认密码: ";
        confirm_password = PlatformUtils::getHiddenInput();
        
        if (password != confirm_password) {
            std::cout << "两次输入的密码不一致！\n";
            return false;
        }
        
        std::cout << "请选择用户类型 (1.消费者 2.商家): ";
        int typeChoice = Menu::getChoice(1, 2);
        type = (typeChoice == 1) ? "消费者" : "商家";
        
        return userManager.registerUser(username, password, type);
    }

    // 处理商品浏览功能（带分页）
    void handleBrowseProducts(ProductManager& productManager) {
        auto products = productManager.getAllProducts();
        if (products.empty()) {
            std::cout << "\n暂无商品\n";
            return;
        }

        int totalPages = (products.size() + ITEMS_PER_PAGE - 1) / ITEMS_PER_PAGE;
        int currentPage = 1;

        while (true) {
            Menu::clearScreen();
            std::cout << "\n=== 商品列表 (第 " << currentPage << "/" << totalPages << " 页) ===\n";
            
            // 计算当前页的起始和结束索引
            int startIdx = (currentPage - 1) * ITEMS_PER_PAGE;
            int endIdx = std::min(startIdx + ITEMS_PER_PAGE, static_cast<int>(products.size()));
            
            // 显示当前页的商品
            for (int i = startIdx; i < endIdx; ++i) {
                products[i]->display();
                std::cout << "----------------\n";
            }
            
            // 显示导航选项
            std::cout << "\n操作说明:\n";
            std::cout << "1. 下一页\n";
            std::cout << "2. 上一页\n";
            std::cout << "3. 跳转到指定页\n";
            std::cout << "0. 返回\n";
            
            int choice = Menu::getChoice(0, 3);
            switch (choice) {
                case 0:
                    return;
                case 1:
                    if (currentPage < totalPages) {
                        currentPage++;
                    } else {
                        std::cout << "已经是最后一页了\n";
                        waitForKey();
                    }
                    break;
                case 2:
                    if (currentPage > 1) {
                        currentPage--;
                    } else {
                        std::cout << "已经是第一页了\n";
                        waitForKey();
                    }
                    break;
                case 3:
                    std::cout << "请输入页码 (1-" << totalPages << "): ";
                    int pageNum;
                    std::cin >> pageNum;
                    if (pageNum >= 1 && pageNum <= totalPages) {
                        currentPage = pageNum;
                    } else {
                        std::cout << "无效的页码\n";
                        waitForKey();
                    }
                    break;
            }
        }
    }

    // 处理消费者余额管理
    void handleBalance(UserManager& userManager, const std::string& username) {
        double balance = userManager.getBalance(username);
        std::cout << "\n当前余额: " << balance << " 元\n";
        std::cout << "1. 充值\n";
        std::cout << "2. 返回\n";
        
        if (Menu::getChoice(1, 2) == 1) {
            std::cout << "请输入充值金额: ";
            double amount;
            std::cin >> amount;
            if (amount > 0 && userManager.updateBalance(username, balance + amount)) {
                std::cout << "充值成功！\n";
            } else {
                std::cout << "充值失败！\n";
            }
        }
    }

    // 处理商家添加商品
    void handleAddProduct(ProductManager& productManager, const std::string& sellerUsername) {
        std::string name, category, description;
        double price;
        int quantity;

        std::cout << "\n=== 添加新商品 ===\n";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  // 清理之前的缓冲区
        
        std::cout << "商品名称: ";
        std::getline(std::cin, name);
        
        std::cout << "商品类别 (1.图书 2.食品 3.服装): ";
        int categoryChoice = Menu::getChoice(1, 3);
        switch(categoryChoice) {
            case 1: category = "图书"; break;
            case 2: category = "食品"; break;
            case 3: category = "服装"; break;
        }
        
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "商品描述: ";
        std::getline(std::cin, description);
        
        std::cout << "商品价格: ";
        while (!(std::cin >> price) || price <= 0) {
            std::cout << "价格必须为正数，请重新输入: ";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        
        std::cout << "商品数量: ";
        while (!(std::cin >> quantity) || quantity < 0) {
            std::cout << "数量必须为非负整数，请重新输入: ";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        
        if (productManager.addProduct(name, category, description, price, quantity, sellerUsername)) {
            std::cout << "商品添加成功！\n";
        } else {
            std::cout << "商品添加失败！\n";
        }
    }

    void handleChangePassword(UserManager& userManager, const std::string& username) {
        std::cout << "\n=== 修改密码 ===\n";
        
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  // 清除之前的输入缓冲
        
        std::cout << "请输入当前密码: ";
        std::string oldPassword = PlatformUtils::getHiddenInput();
        
        std::cout << "请输入新密码: ";
        std::string newPassword = PlatformUtils::getHiddenInput();
        
        std::cout << "请确认新密码: ";
        std::string confirmPassword = PlatformUtils::getHiddenInput();
        
        if (newPassword != confirmPassword) {
            std::cout << "两次输入的新密码不一致！\n";
            return;
        }
        
        if (userManager.changePassword(username, oldPassword, newPassword)) {
            std::cout << "密码修改成功！\n";
        } else {
            std::cout << "密码修改失败：当前密码可能输入错误\n";
        }
    }
}

void Menu::showMainMenu() {
  clearScreen();
  std::cout << "\n┌────────────────────────────────┐\n";
  std::cout << "│        电商交易平台主菜单      │\n";
  std::cout << "├────────────────────────────────┤\n";
  std::cout << "│  1. 用户登录                   │\n";
  std::cout << "│  2. 用户注册                   │\n";
  std::cout << "│  3. 浏览所有商品               │\n";
  std::cout << "│  0. 退出系统                   │\n";
  std::cout << "└────────────────────────────────┘\n";
}

void Menu::displayMainMenu() {
    UserManager userManager;
    ProductManager productManager;
    
    while(true) {
        showMainMenu();
        int choice = getChoice(0, 3);
        
        switch(choice) {
            case 0: // 退出
                std::cout << "感谢使用，再见！\n";
                return;
                
            case 1: { // 用户登录
                auto currentUser = handleLogin(userManager);
                if (currentUser) {
                    displayUserMenu(currentUser);
                }
                waitForKey();
                break;
            }
                
            case 2: { // 用户注册
                bool success = handleRegister(userManager);
                if (success) {
                    std::cout << "注册成功！\n";
                } else {
                    std::cout << "注册失败：用户名可能已存在\n";
                }
                waitForKey();
                break;
            }
                
            case 3: { // 浏览商品
                handleBrowseProducts(productManager);
                waitForKey();
                break;
            }
        }
    }
}

void Menu::displayUserMenu(const std::shared_ptr<User>& currentUser) {
    UserManager userManager;
    ProductManager productManager;
    
    while (true) {
        clearScreen();
        if (currentUser->getUserType() == "消费者") {
            showConsumerMenu();
            int choice = getChoice(0, 6);
            
            switch(choice) {
                case 0: return;  // 退出登录
                case 1: 
                    handleBrowseProducts(productManager);
                    break;
                case 2: {
                    std::cout << "请输入搜索关键词: ";
                    std::string keyword;
                    std::cin.ignore();
                    std::getline(std::cin, keyword);
                    auto products = productManager.searchProducts(keyword);
                    if (products.empty()) {
                        std::cout << "未找到相关商品\n";
                    } else {
                        int currentPage = 1;
                        int totalPages = (products.size() + ITEMS_PER_PAGE - 1) / ITEMS_PER_PAGE;
                        
                        while (true) {
                            clearScreen();
                            std::cout << "\n=== 搜索结果 (第 " << currentPage << "/" << totalPages << " 页) ===\n";
                            std::cout << "关键词: " << keyword << "\n\n";
                            
                            int startIdx = (currentPage - 1) * ITEMS_PER_PAGE;
                            int endIdx = std::min(startIdx + ITEMS_PER_PAGE, static_cast<int>(products.size()));
                            
                            for (int i = startIdx; i < endIdx; ++i) {
                                products[i]->display();
                                std::cout << "----------------\n";
                            }
                            
                            std::cout << "\n操作说明:\n";
                            std::cout << "1. 下一页\n";
                            std::cout << "2. 上一页\n";
                            std::cout << "3. 跳转到指定页\n";
                            std::cout << "0. 返回\n";
                            
                            int choice = Menu::getChoice(0, 3);
                            switch (choice) {
                                case 0:
                                    return;
                                case 1:
                                    if (currentPage < totalPages) {
                                        currentPage++;
                                    } else {
                                        std::cout << "已经是最后一页了\n";
                                        waitForKey();
                                    }
                                    break;
                                case 2:
                                    if (currentPage > 1) {
                                        currentPage--;
                                    } else {
                                        std::cout << "已经是第一页了\n";
                                        waitForKey();
                                    }
                                    break;
                                case 3:
                                    std::cout << "请输入页码 (1-" << totalPages << "): ";
                                    int pageNum;
                                    std::cin >> pageNum;
                                    if (pageNum >= 1 && pageNum <= totalPages) {
                                        currentPage = pageNum;
                                    } else {
                                        std::cout << "无效的页码\n";
                                        waitForKey();
                                    }
                                    break;
                            }
                        }
                    }
                    break;
                }
                case 3:
                    std::cout << "购物车功能开发中...\n";
                    break;
                case 4:
                    std::cout << "订单历史功能开发中...\n";
                    break;
                case 5:
                    handleBalance(userManager, currentUser->getUsername());
                    break;
                case 6:
                    handleChangePassword(userManager, currentUser->getUsername());
                    break;
            }
        } else if (currentUser->getUserType() == "商家") {
            showSellerMenu();
            int choice = getChoice(0, 5);
            
            switch(choice) {
                case 0: return;  // 退出登录
                case 1:
                    handleAddProduct(productManager, currentUser->getUsername());
                    break;
                case 2:
                    std::cout << "商品管理功能开发中...\n";
                    break;
                case 3:
                    std::cout << "销售统计功能开发中...\n";
                    break;
                case 4:
                    handleBalance(userManager, currentUser->getUsername());
                    break;
                case 5:
                    handleChangePassword(userManager, currentUser->getUsername());
                    break;
            }
        }
        waitForKey();
    }
}

void Menu::showConsumerMenu() {
  std::cout << "\n==== 消费者菜单 ====\n";
  std::cout << "1. 查看商品列表\n";
  std::cout << "2. 搜索商品\n";
  std::cout << "3. 查看购物车\n";
  std::cout << "4. 查看订单历史\n";
  std::cout << "5. 账户余额管理\n";
  std::cout << "6. 修改密码\n";
  std::cout << "0. 退出登录\n";
  std::cout << "请选择操作 [0-6]: ";
}

void Menu::showSellerMenu() {
  std::cout << "\n==== 商家菜单 ====\n";
  std::cout << "1. 添加新商品\n";
  std::cout << "2. 管理商品\n";
  std::cout << "3. 查看销售统计\n";
  std::cout << "4. 账户余额查询\n";
  std::cout << "5. 修改密码\n";
  std::cout << "0. 退出登录\n";
  std::cout << "请选择操作 [0-5]: ";
}

int Menu::getChoice(int min, int max) {
  int choice;
  while (true) {
      if (std::cin >> choice && choice >= min && choice <= max) {
          return choice;
      }
      std::cin.clear();
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      std::cout << "输入无效，请重新输入 [" << min << "-" << max << "]: ";
  }
}

void Menu::clearScreen() {
    PlatformUtils::clearScreen();
}