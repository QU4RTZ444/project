#include "include.h"
#include "menu.h"
#include "menu_handler.h"
#include "platform_utils.h"

const int ITEMS_PER_PAGE = 3;  // 每页显示的商品数量

void MenuHandler::waitForKey() const {
    std::cout << "\n按回车键继续...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

std::shared_ptr<User> MenuHandler::handleLogin() {
    std::string username, password;
    std::cout << "\n=== 用户登录 ===\n";
    std::cout << "请输入用户名: ";
    std::cin >> username;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    
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

bool MenuHandler::handleRegister() {
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

void MenuHandler::handleBrowseProducts() const {
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
            case 0: return;
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

void MenuHandler::handleBalance(const std::string& username) {
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

void MenuHandler::handleAddProduct(const std::string& sellerUsername) {
    std::string name, category, description;
    double price;
    int quantity;

    std::cout << "\n=== 添加新商品 ===\n";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    
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

void MenuHandler::handleChangePassword(const std::string& username) {
    std::cout << "\n=== 修改密码 ===\n";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    
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

void MenuHandler::displayUserMenu(const std::shared_ptr<User>& currentUser) {
    while (true) {
        Menu::clearScreen();
        if (currentUser->getUserType() == "消费者") {
            Menu::showConsumerMenu();
            if (handleConsumerChoice(currentUser)) {
                return;
            }
        } else if (currentUser->getUserType() == "商家") {
            Menu::showSellerMenu();
            handleSellerChoice(currentUser);
        }
        waitForKey();
    }
}

bool MenuHandler::handleConsumerChoice(const std::shared_ptr<User>& currentUser) {
    int choice = Menu::getChoice(0, 4);
    switch(choice) {
        case 0: 
            return true;  // 返回上层菜单
        case 1: 
            handleBrowseProducts();
            break;
        case 2:
            handleSearchProducts();
            break;
        case 3:
            handleBalance(currentUser->getUsername());
            break;
        case 4:
            handleChangePassword(currentUser->getUsername());
            break;
    }
    return false;  // 继续当前菜单
}

bool MenuHandler::handleSellerChoice(const std::shared_ptr<User>& currentUser) {
    int choice = Menu::getChoice(0, 5);
    switch(choice) {
        case 0: return true;  // 返回上层菜单
        case 1:
            handleAddProduct(currentUser->getUsername());
            break;
        case 2:
            handleManageProducts(currentUser->getUsername());
            break;
        case 3:
            handleBalance(currentUser->getUsername());
            break;
        case 4:
            handleChangePassword(currentUser->getUsername());
            break;
    }
    return false;  // 继续当前菜单
}

void MenuHandler::handleSearchProducts() const {
    while (true) {
        Menu::clearScreen();
        std::cout << "\n=== 商品搜索 ===\n";
        std::cout << "1. 按名称搜索\n";
        std::cout << "2. 按类别筛选\n";
        std::cout << "3. 按价格区间筛选\n";
        std::cout << "0. 返回\n";
        
        int choice = Menu::getChoice(0, 3);
        if (choice == 0) return;
        
        std::vector<std::shared_ptr<Product>> products;
        
        switch (choice) {
            case 1: {
                std::cout << "请输入商品名称关键词: ";
                std::string keyword;
                std::cin.ignore();
                std::getline(std::cin, keyword);
                
                products = productManager.searchProducts(keyword);
                displaySearchResults(products, "名称包含 '" + keyword + "'");
                break;
            }
            case 2: {
                std::cout << "选择商品类别：\n";
                std::cout << "1. 图书\n";
                std::cout << "2. 食品\n";
                std::cout << "3. 服装\n";
                int categoryChoice = Menu::getChoice(1, 3);
                
                std::string category;
                switch(categoryChoice) {
                    case 1: category = "图书"; break;
                    case 2: category = "食品"; break;
                    case 3: category = "服装"; break;
                }
                
                products = productManager.searchByCategory(category);
                displaySearchResults(products, "类别为 " + category);
                break;
            }
            case 3: {
                double minPrice, maxPrice;
                std::cout << "请输入最低价格: ";
                while (!(std::cin >> minPrice) || minPrice < 0) {
                    std::cout << "价格必须为非负数，请重新输入: ";
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                }
                
                std::cout << "请输入最高价格: ";
                while (!(std::cin >> maxPrice) || maxPrice < minPrice) {
                    std::cout << "价格必须大于最低价格，请重新输入: ";
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                }
                
                products = productManager.searchByPriceRange(minPrice, maxPrice);
                displaySearchResults(products, 
                    "价格在 " + std::to_string(minPrice) + 
                    " - " + std::to_string(maxPrice) + " 之间");
                break;
            }
        }
    }
}

void MenuHandler::displaySearchResults(const std::vector<std::shared_ptr<Product>>& products,
                                    const std::string& searchCriteria) const {
    if (products.empty()) {
        std::cout << "\n未找到" << searchCriteria << "的商品\n";
        waitForKey();
        return;
    }

    int totalPages = (products.size() + ITEMS_PER_PAGE - 1) / ITEMS_PER_PAGE;
    int currentPage = 1;

    while (true) {
        Menu::clearScreen();
        std::cout << "\n=== 搜索结果 (第 " << currentPage << "/" << totalPages << " 页) ===\n";
        std::cout << "筛选条件: " << searchCriteria << "\n\n";
        
        int startIdx = (currentPage - 1) * ITEMS_PER_PAGE;
        int endIdx = std::min(startIdx + ITEMS_PER_PAGE, static_cast<int>(products.size()));
        
        for (int i = startIdx; i < endIdx; ++i) {
            products[i]->display();
            std::cout << "----------------\n";
        }
        
        std::cout << "\n操作说明:\n";
        std::cout << "1. 下一页\n";
        std::cout << "2. 上一页\n";
        std::cout << "3. 跳转页面\n";
        std::cout << "0. 返回\n";
        
        int choice = Menu::getChoice(0, 3);
        if (choice == 0) break;
        
        handlePageNavigation(choice, currentPage, totalPages);
    }
}

bool MenuHandler::handlePageNavigation(int choice, int& currentPage, int totalPages) const {
    switch (choice) {
        case 1:
            if (currentPage < totalPages) {
                currentPage++;
            } else {
                std::cout << "已经是最后一页了\n";
                waitForKey();
            }
            return true;
        case 2:
            if (currentPage > 1) {
                currentPage--;
            } else {
                std::cout << "已经是第一页了\n";
                waitForKey();
            }
            return true;
        case 3:
            std::cout << "请输入页码 (1-" << totalPages << "): ";
            int pageNum;
            if (std::cin >> pageNum && pageNum >= 1 && pageNum <= totalPages) {
                currentPage = pageNum;
            } else {
                std::cout << "无效的页码\n";
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                waitForKey();
            }
            return true;
        default:
            return false;
    }
}

void MenuHandler::handleManageProducts(const std::string& sellerUsername) {
    try {
        while (true) {
            Menu::clearScreen();
            std::cout << "\n=== 商品管理 ===\n";
            
            #ifdef DEBUG
            std::cout << "正在获取商品列表...\n";
            #endif
            auto products = productManager.getSellerProducts(sellerUsername);
            #ifdef DEBUG
            std::cout << "获取到 " << products.size() << " 个商品\n";
            #endif
            
            if (products.empty()) {
                std::cout << "您还没有添加任何商品\n";
                waitForKey();
                return;
            }

            // 添加分页相关变量
            int totalPages = (products.size() + ITEMS_PER_PAGE - 1) / ITEMS_PER_PAGE;
            int currentPage = 1;

            while (true) {
                Menu::clearScreen();
                std::cout << "\n=== 商品管理 (第 " << currentPage << "/" << totalPages << " 页) ===\n";
                
                // 计算当前页的商品范围
                int startIdx = (currentPage - 1) * ITEMS_PER_PAGE;
                int endIdx = std::min(startIdx + ITEMS_PER_PAGE, static_cast<int>(products.size()));
                
                // 显示当前页的商品
                for (int i = startIdx; i < endIdx; ++i) {
                    std::cout << "\n[" << i + 1 << "] ";
                    try {
                        if (!products[i]) {
                            throw std::runtime_error("商品指针为空");
                        }
                        std::cout << "================================\n";
                        products[i]->display();
                        std::cout << "================================\n";
                    } catch (const std::exception& e) {
                        std::cerr << "显示商品 " << (i + 1) << " 时出错: " << e.what() << std::endl;
                        continue;
                    }
                }
                
                // 显示操作菜单
                std::cout << "\n请选择操作：\n";
                std::cout << "1. 修改商品信息\n";
                std::cout << "2. 更新商品价格\n";
                std::cout << "3. 更新商品库存\n";
                std::cout << "4. 下架商品\n";
                std::cout << "5. 设置商品折扣\n";
                std::cout << "6. 设置品类折扣\n";
                std::cout << "7. 下一页\n";
                std::cout << "8. 上一页\n";
                std::cout << "9. 跳转页面\n";
                std::cout << "0. 返回\n";
                
                int choice = Menu::getChoice(0, 9);
                if (choice == 0) return;
                
                // 处理分页操作
                if (choice >= 7 && choice <= 9) {
                    switch (choice) {
                        case 7:
                            if (currentPage < totalPages) {
                                currentPage++;
                            } else {
                                std::cout << "已经是最后一页了\n";
                                waitForKey();
                            }
                            continue;
                        case 8:
                            if (currentPage > 1) {
                                currentPage--;
                            } else {
                                std::cout << "已经是第一页了\n";
                                waitForKey();
                            }
                            continue;
                        case 9:
                            std::cout << "请输入页码 (1-" << totalPages << "): ";
                            int pageNum;
                            if (std::cin >> pageNum && pageNum >= 1 && pageNum <= totalPages) {
                                currentPage = pageNum;
                            } else {
                                std::cout << "无效的页码\n";
                                std::cin.clear();
                                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                                waitForKey();
                            }
                            continue;
                    }
                }

                // 处理品类折扣设置
                if (choice == 6) {
                    std::cout << "\n=== 设置品类折扣 ===\n";
                    std::cout << "选择要设置折扣的品类：\n";
                    std::cout << "1. 图书\n";
                    std::cout << "2. 食品\n";
                    std::cout << "3. 服装\n";
                    std::cout << "0. 返回\n";
                    
                    int categoryChoice = Menu::getChoice(0, 3);
                    if (categoryChoice == 0) {
                        continue;
                    }
                    
                    std::string category;
                    switch(categoryChoice) {
                        case 1: category = "图书"; break;
                        case 2: category = "食品"; break;
                        case 3: category = "服装"; break;
                        default: 
                            std::cout << "无效的选择\n";
                            continue;
                    }
                    
                    std::cout << "请输入折扣率 (0-1，如0.8表示8折): ";
                    double discountRate;
                    if (std::cin >> discountRate && discountRate > 0 && discountRate <= 1) {
                        #ifdef DEBUG
                        std::cout << "正在设置品类折扣...\n";
                        #endif
                        
                        if (productManager.setCategoryDiscount(category, discountRate, sellerUsername)) {
                            std::cout << "已成功为所有" << category << "类商品设置" 
                                     << (discountRate * 10) << "折优惠\n";
                        } else {
                            std::cout << "设置品类折扣失败\n";
                        }
                    } else {
                        std::cout << "无效的折扣率\n";
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    }
                    continue;
                }
                
                // 选择商品
                std::cout << "请输入商品序号 (1-" << products.size() << "): ";
                size_t index;
                if (!(std::cin >> index) || index < 1 || index > products.size()) {
                    std::cout << "无效的商品序号\n";
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    waitForKey();
                    continue;
                }
                
                auto& product = products[index - 1];
                if (!product) {
                    std::cout << "商品数据无效\n";
                    waitForKey();
                    continue;
                }
                
                switch (choice) {
                    case 1: {
                        std::string newName, newDescription;
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        
                        std::cout << "\n当前商品信息：\n";
                        std::cout << "名称：" << product->getName() << "\n";
                        std::cout << "描述："; 
                        product->display();
                        
                        std::cout << "\n请输入新的商品名称（直接回车保持不变）：";
                        std::getline(std::cin, newName);
                        if (newName.empty()) {
                            newName = product->getName();
                        }
                        
                        std::cout << "请输入新的商品描述（直接回车保持不变）：";
                        std::getline(std::cin, newDescription);
                        if (newDescription.empty()) {
                            std::cout << "描述未更改\n";
                            break;
                        }
                        
                        if (productManager.updateProductInfo(product->getId(), 
                                                           newName,
                                                           newDescription,
                                                           sellerUsername)) {
                            std::cout << "商品信息更新成功！\n";
                        } else {
                            std::cout << "商品信息更新失败！\n";
                        }
                        break;
                    }
                    case 2: {
                        std::cout << "请输入新价格: ";
                        double newPrice;
                        if (std::cin >> newPrice && newPrice > 0) {
                            if (productManager.updatePrice(product->getId(), newPrice, sellerUsername)) {
                                std::cout << "价格更新成功\n";
                            } else {
                                std::cout << "价格更新失败\n";
                            }
                        } else {
                            std::cout << "无效的价格\n";
                            std::cin.clear();
                            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        }
                        break;
                    }
                    case 3: {
                        std::cout << "请输入新库存数量: ";
                        int newQuantity;
                        if (std::cin >> newQuantity && newQuantity >= 0) {
                            if (productManager.updateQuantity(product->getId(), newQuantity, sellerUsername)) {
                                std::cout << "库存更新成功\n";
                            } else {
                                std::cout << "库存更新失败\n";
                            }
                        } else {
                            std::cout << "无效的库存数量\n";
                            std::cin.clear();
                            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        }
                        break;
                    }
                    case 4: {
                        std::cout << "确认要下架该商品吗？(1:是 0:否): ";
                        int confirm;
                        if (std::cin >> confirm && confirm == 1) {
                            if (productManager.removeProduct(product->getId(), sellerUsername)) {
                                std::cout << "商品已下架\n";
                                break;
                            } else {
                                std::cout << "下架失败\n";
                            }
                        }
                        break;
                    }
                    case 5: {
                        std::cout << "请输入折扣率 (0-1，如0.8表示8折): ";
                        double discountRate;
                        if (std::cin >> discountRate && discountRate > 0 && discountRate <= 1) {
                            if (productManager.setProductDiscount(product->getId(), 
                                                               discountRate, 
                                                               sellerUsername)) {
                                std::cout << "折扣设置成功\n";
                            } else {
                                std::cout << "折扣设置失败\n";
                            }
                        } else {
                            std::cout << "无效的折扣率\n";
                            std::cin.clear();
                            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        }
                        break;
                    }
                }
                
                waitForKey();
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "商品管理功能发生错误: " << e.what() << std::endl;
        waitForKey();
    }
}
