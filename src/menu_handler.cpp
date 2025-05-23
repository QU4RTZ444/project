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

void MenuHandler::handleBrowseProducts(const std::shared_ptr<User>& currentUser) {
    bool canBuy = (currentUser != nullptr);  // 检查是否是已登录用户
    
    while (true) {
        Menu::clearScreen();
        std::cout << "\n=== 浏览商品 ===\n";
        
        auto products = productManager.getAllProducts();
        if (products.empty()) {
            std::cout << "暂无商品\n";
            waitForKey();
            return;
        }

        int totalPages = (products.size() + ITEMS_PER_PAGE - 1) / ITEMS_PER_PAGE;
        int currentPage = 1;

        while (true) {
            Menu::clearScreen();
            std::cout << "\n=== 商品列表 (第 " << currentPage << "/" << totalPages << " 页) ===\n";
            
            int startIdx = (currentPage - 1) * ITEMS_PER_PAGE;
            int endIdx = std::min(startIdx + ITEMS_PER_PAGE, static_cast<int>(products.size()));
            
            // 显示当前页的商品
            for (int i = startIdx; i < endIdx; ++i) {
                std::cout << "\n[" << (i + 1) << "] ";
                products[i]->display();
                std::cout << "----------------\n";
            }
            
            // 根据用户登录状态显示不同的操作菜单
            std::cout << "\n操作说明:\n";
            if (canBuy) {
                std::cout << "1-" << (endIdx - startIdx) << ". 选择商品加入购物车\n";
            }
            std::cout << "N. 下一页\n";
            std::cout << "P. 上一页\n";
            std::cout << "J. 跳转到指定页\n";
            std::cout << "Q. 返回\n";

            char choice;
            std::cout << "请选择: ";
            std::cin >> choice;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            if (toupper(choice) == 'Q') return;
            
            if (toupper(choice) == 'N') {
                if (currentPage < totalPages) {
                    currentPage++;
                } else {
                    std::cout << "已经是最后一页了\n";
                    waitForKey();
                }
                continue;
            }
            
            if (toupper(choice) == 'P') {
                if (currentPage > 1) {
                    currentPage--;
                } else {
                    std::cout << "已经是第一页了\n";
                    waitForKey();
                }
                continue;
            }
            
            if (toupper(choice) == 'J') {
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

            // 处理商品选择（仅对已登录用户有效）
            if (canBuy) {
                int index = choice - '1';
                if (index >= 0 && index < (endIdx - startIdx)) {
                    auto product = products[startIdx + index];
                    handleAddToCart(product, currentUser->getUsername());
                    waitForKey();
                }
            } else if (isdigit(choice)) {
                std::cout << "请先登录后再添加商品到购物车\n";
                waitForKey();
            }
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

void MenuHandler::handleAddToCart(const std::shared_ptr<Product>& product, 
                                const std::string& username) {
    if (!product) {
        std::cout << "无效的商品\n";
        return;
    }

    std::cout << "请输入购买数量: ";
    int quantity;
    if (std::cin >> quantity && quantity > 0) {
        try {
            ShoppingCart cart(username);
            // 从数据库加载现有购物车
            auto cartItems = db->getCartItems(username);
            for (const auto& [productId, qty] : cartItems) {
                auto p = productManager.getProduct(productId);
                if (p) {
                    cart.addItem(p, qty);
                }
            }

            // 添加新商品
            if (cart.addItem(product, quantity)) {
                if (db->saveCartItem(username, product->getId(), quantity)) {
                    std::cout << "成功添加到购物车！\n";
                } else {
                    std::cout << "保存到数据库失败\n";
                }
            }
        } catch (const std::exception& e) {
            std::cout << "添加失败: " << e.what() << "\n";
        }
    } else {
        std::cout << "无效的数量\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    waitForKey();
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
    while (true) {
        Menu::clearScreen();
        std::cout << "\n=== 消费者菜单 ===\n";
        std::cout << "1. 浏览商品\n";
        std::cout << "2. 搜索商品\n";
        std::cout << "3. 购物车\n";
        std::cout << "4. 我的订单\n";
        std::cout << "5. 账户信息\n";
        std::cout << "0. 退出登录\n";

        int choice = Menu::getChoice(0, 5);
        
        try {
            switch (choice) {
                case 0: return true;
                case 1:
                    handleBrowseProducts(currentUser);
                    break;
                case 2:
                    handleSearchProducts(currentUser);
                    break;
                case 3:
                    handleShoppingCart(currentUser->getUsername());
                    break;
                case 4:
                    handleOrderManagement(currentUser->getUsername());
                    break;
                case 5:
                    handleAccountInfo(currentUser);
                    break;
            }
        } catch (const std::exception& e) {
            std::cerr << "操作失败: " << e.what() << "\n";
            waitForKey();
        }
    }
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

void MenuHandler::handleSearchProducts(const std::shared_ptr<User>& currentUser) {
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
                displaySearchResults(products, "名称包含 '" + keyword + "'", currentUser->getUsername());
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
                    default: continue;  // 无效选择，跳过
                }
                
                products = productManager.searchByCategory(category);
                displaySearchResults(products, "类别为 " + category, currentUser->getUsername());
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
                    " - " + std::to_string(maxPrice) + " 之间", currentUser->getUsername());
                break;
            }
        }
    }
}

void MenuHandler::displaySearchResults(const std::vector<std::shared_ptr<Product>>& products,
                                    const std::string& searchCriteria,
                                    const std::string& username) {
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
            std::cout << "\n[" << (i + 1) << "] ";
            products[i]->display();
            std::cout << "----------------\n";
        }
        
        std::cout << "\n操作说明:\n";
        std::cout << "1-" << (endIdx - startIdx) << ". 选择商品加入购物车\n";
        std::cout << "N. 下一页\n";
        std::cout << "P. 上一页\n";
        std::cout << "Q. 返回\n";
        
        char choice;
        std::cout << "请选择: ";
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (toupper(choice) == 'Q') break;
        
        if (toupper(choice) == 'N') {
            if (currentPage < totalPages) currentPage++;
            continue;
        }
        
        if (toupper(choice) == 'P') {
            if (currentPage > 1) currentPage--;
            continue;
        }
        
        // 处理商品选择
        int index = choice - '1';
        if (index >= 0 && index < (endIdx - startIdx)) {
            auto product = products[startIdx + index];
            if (!username.empty()) {  // 仅登录用户可添加到购物车
                handleAddToCart(product, username);
            } else {
                std::cout << "请先登录后再添加商品到购物车\n";
                waitForKey();
            }
        }
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

bool MenuHandler::displayCart(const ShoppingCart& cart) const {
    const auto& items = cart.getItems();
    if (items.empty()) {
        std::cout << "购物车为空\n";
        return false;
    }

    std::cout << "\n=== 购物车内容 ===\n";
    double total = 0.0;
    for (const auto& item : items) {
        auto product = item.getProduct();
        std::cout << "\n--------------------------------\n";
        product->display();
        std::cout << "数量: " << item.getQuantity() << "\n";
        
        double discountedPrice = product->getPrice();
        double itemTotal = discountedPrice * item.getQuantity();
        
        if (product->getDiscountRate() < 1.0) {
            double originalTotal = product->getBasePrice() * item.getQuantity();
            std::cout << "原价小计: ￥" << std::fixed << std::setprecision(2) 
                     << originalTotal << "\n";
            std::cout << "折后小计: ￥" << std::fixed << std::setprecision(2) 
                     << itemTotal << "\n";
        } else {
            std::cout << "小计: ￥" << std::fixed << std::setprecision(2) 
                     << itemTotal << "\n";
        }
        total += itemTotal;
    }
    
    std::cout << "\n--------------------------------\n";
    std::cout << "总计: ￥" << std::fixed << std::setprecision(2) << total << "\n";
    return true;
}

void MenuHandler::handleUpdateCartItem(ShoppingCart& cart) {
    if (!displayCart(cart)) {
        return;
    }

    std::cout << "\n请输入要修改的商品ID: ";
    int productId;
    if (!(std::cin >> productId)) {
        std::cout << "无效的商品ID\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return;
    }

    std::cout << "请输入新的数量: ";
    int newQuantity;
    if (std::cin >> newQuantity && newQuantity > 0) {
        try {
            if (cart.updateQuantity(productId, newQuantity)) {
                std::cout << "数量更新成功\n";
            } else {
                std::cout << "商品不在购物车中\n";
            }
        } catch (std::runtime_error& e) {
            std::cout << "更新失败: " << e.what() << "\n";
        }
    } else {
        std::cout << "无效的数量\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

void MenuHandler::handleRemoveCartItem(ShoppingCart& cart) {
    if (!displayCart(cart)) {
        return;
    }

    std::cout << "\n请输入要删除的商品ID: ";
    int productId;
    if (std::cin >> productId) {
        if (cart.removeItem(productId)) {
            // 同步更新数据库
            if (db->removeCartItem(cart.getUsername(), productId)) {
                std::cout << "商品已从购物车移除\n";
            } else {
                std::cout << "商品移除失败\n";
            }
        } else {
            std::cout << "商品不在购物车中\n";
        }
    } else {
        std::cout << "无效的商品ID\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

bool MenuHandler::confirmAction(const std::string& message) const {
    std::cout << message << " (1:是 0:否): ";
    int confirm;
    if (std::cin >> confirm) {
        return confirm == 1;
    }
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return false;
}

void MenuHandler::handleCheckout(ShoppingCart& cart, const std::string& username) {
    if (!displayCart(cart)) {
        return;
    }

    if (!confirmAction("确认结算购物车？")) {
        return;
    }

    try {
        OrderManager orderManager;
        auto order = orderManager.createOrder(cart);
        if (!order) {
            throw std::runtime_error("创建订单失败");
        }

        std::cout << "订单创建成功！订单号: " << order->getId() << "\n";
        std::cout << "订单总额: ￥" << std::fixed << std::setprecision(2) 
                 << order->getTotalAmount() << "\n";
        
        if (confirmAction("是否立即支付？")) {
            try {
                if (orderManager.payOrder(order->getId(), username)) {
                    std::cout << "支付成功！\n";
                    
                    // 支付成功后清空购物车
                    try {
                        if (db->removeCartItem(username, -1)) {
                            cart.clear();
                            std::cout << "购物车已清空\n";
                        } else {
                            std::cout << "清空购物车失败\n";
                        }
                    } catch (const std::exception& e) {
                        std::cerr << "清空购物车失败: " << e.what() << "\n";
                    }
                } else {
                    throw std::runtime_error("支付处理失败");
                }
            } catch (const std::exception& e) {
                // 如果支付失败，尝试取消订单
                try {
                    if (!orderManager.cancelOrder(order->getId(), username)) {
                        std::cerr << "警告：订单取消失败，请联系客服处理\n";
                    }
                } catch (const std::exception& e) {
                    std::cerr << "警告：订单取消失败，请联系客服处理\n";
                }
            }
        } else {
            std::cout << "您可以稍后在\"我的订单\"中完成支付\n";
        }
    } catch (const std::exception& e) {
        std::cout << "结算失败: " << e.what() << "\n";
    }
    waitForKey();
}

void MenuHandler::handleOrderManagement(const std::string& username) {
    OrderManager orderManager;
    
    while (true) {
        Menu::clearScreen();
        std::cout << "\n=== 我的订单 ===\n";
        
        auto orders = orderManager.getUserOrders(username);
        if (orders.empty()) {
            std::cout << "暂无订单\n";
            waitForKey();
            return;
        }

        // 添加分页
        int totalPages = (orders.size() + ITEMS_PER_PAGE - 1) / ITEMS_PER_PAGE;
        static int currentPage = 1;

        std::cout << "\n订单列表 (第 " << currentPage << "/" << totalPages << " 页)：\n";
        
        // 计算当前页的订单范围
        int startIdx = (currentPage - 1) * ITEMS_PER_PAGE;
        int endIdx = std::min(startIdx + ITEMS_PER_PAGE, static_cast<int>(orders.size()));

        // 显示当前页的订单
        for (int i = startIdx; i < endIdx; ++i) {
            const auto& order = orders[i];
            std::cout << "\n================================\n";
            std::cout << "订单号: " << order->getId() << "\n";
            std::cout << "创建时间: " << order->getCreateTime() << "\n";
            std::cout << "订单状态: ";
            switch (order->getStatus()) {
                case OrderStatus::PENDING: std::cout << "待支付\n"; break;
                case OrderStatus::PAID: std::cout << "已支付\n"; break;
                case OrderStatus::CANCELLED: std::cout << "已取消\n"; break;
                case OrderStatus::FAILED: std::cout << "支付失败\n"; break;
            }
            std::cout << "订单金额: ￥" << std::fixed << std::setprecision(2) 
                     << order->getTotalAmount() << "\n";
            std::cout << "================================\n";
        }

        std::cout << "\n请选择操作：\n";
        std::cout << "1. 查看订单详情\n";
        std::cout << "2. 支付订单\n";
        std::cout << "3. 取消订单\n";
        std::cout << "4. 下一页\n";
        std::cout << "5. 上一页\n";
        std::cout << "6. 跳转页面\n";
        std::cout << "0. 返回\n";

        int choice = Menu::getChoice(0, 6);
        if (choice == 0) break;

        try {
            switch (choice) {
                case 4: // 下一页
                    if (currentPage < totalPages) {
                        currentPage++;
                        continue;
                    } else {
                        std::cout << "已经是最后一页了\n";
                    }
                    break;
                case 5: // 上一页
                    if (currentPage > 1) {
                        currentPage--;
                        continue;
                    } else {
                        std::cout << "已经是第一页了\n";
                    }
                    break;
                case 6: // 跳转页面
                    std::cout << "请输入页码 (1-" << totalPages << "): ";
                    int pageNum;
                    if (std::cin >> pageNum && pageNum >= 1 && pageNum <= totalPages) {
                        currentPage = pageNum;
                        continue;
                    } else {
                        std::cout << "无效的页码\n";
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    }
                    break;
                default: {
                    std::cout << "请输入订单号: ";
                    int orderId;
                    if (!(std::cin >> orderId)) {
                        std::cout << "无效的订单号\n";
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        continue;
                    }

                    auto order = orderManager.getOrder(orderId);
                    if (!order || order->getBuyerUsername() != username) {
                        std::cout << "订单不存在\n";
                        continue;
                    }

                    switch (choice) {
                        case 1:
                            displayOrderDetails(order);
                            break;
                        case 2:
                            if (order->getStatus() == OrderStatus::PENDING) {
                                if (orderManager.payOrder(orderId, username)) {
                                    std::cout << "支付成功！\n";
                                }
                            } else {
                                std::cout << "该订单不可支付\n";
                            }
                            break;
                        case 3:
                            if (order->getStatus() == OrderStatus::PENDING) {
                                if (orderManager.cancelOrder(orderId, username)) {
                                    std::cout << "订单已取消\n";
                                }
                            } else {
                                std::cout << "该订单不可取消\n";
                            }
                            break;
                    }
                }
            }
        } catch (const std::exception& e) {
            std::cerr << "操作失败: " << e.what() << "\n";
        }
        waitForKey();
    }
}

void MenuHandler::displayOrderDetails(const std::shared_ptr<Order>& order) {
    Menu::clearScreen();
    std::cout << "\n=== 订单详情 ===\n";
    std::cout << "订单号: " << order->getId() << "\n";
    std::cout << "创建时间: " << order->getCreateTime() << "\n";
    std::cout << "订单状态: ";
    switch (order->getStatus()) {
        case OrderStatus::PENDING: std::cout << "待支付\n"; break;
        case OrderStatus::PAID: std::cout << "已支付\n"; break;
        case OrderStatus::CANCELLED: std::cout << "已取消\n"; break;
        case OrderStatus::FAILED: std::cout << "支付失败\n"; break;
    }
    
    std::cout << "\n商品列表：\n";
    for (const auto& item : order->getItems()) {
        std::cout << "\n--------------------------------\n";
        item.getProduct()->display();
        std::cout << "数量: " << item.getQuantity() << "\n";
        double itemTotal = item.getProduct()->getPrice() * item.getQuantity();
        std::cout << "小计: ￥" << std::fixed << std::setprecision(2) << itemTotal << "\n";
    }
    
    std::cout << "\n================================\n";
    std::cout << "订单总额: ￥" << std::fixed << std::setprecision(2) 
              << order->getTotalAmount() << "\n";
    std::cout << "================================\n";
}

void MenuHandler::handleAccountInfo(const std::shared_ptr<User>& currentUser) {
    while (true) {
        Menu::clearScreen();
        std::string username = currentUser->getUsername();
        double balance = userManager.getBalance(username);
        
        std::cout << "\n=== 账户信息 ===\n";
        std::cout << "用户名: " << username << "\n";
        std::cout << "用户类型: " << currentUser->getUserType() << "\n";
        std::cout << "账户余额: ￥" << std::fixed << std::setprecision(2) << balance << "\n";

        std::cout << "\n请选择操作：\n";
        std::cout << "1. 充值\n";
        std::cout << "2. 修改密码\n";
        std::cout << "0. 返回\n";

        int choice = Menu::getChoice(0, 2);
        if (choice == 0) break;

        switch (choice) {
            case 1:
                handleBalance(username);  // 复用现有的余额管理功能
                break;
            case 2:
                handleChangePassword(username);  // 复用现有的密码修改功能
                break;
        }
        waitForKey();
    }
}

void MenuHandler::handleShoppingCart(const std::string& username) {
    ShoppingCart cart(username);

    // 从数据库加载购物车商品
    auto cartItems = db->getCartItems(username);
    for (const auto& [productId, quantity] : cartItems) {
        try {
            auto product = productManager.getProduct(productId);
            if (product) {
                cart.addItem(product, quantity);
            }
        } catch (const std::exception& e) {
            std::cerr << "加载商品失败: " << e.what() << "\n";
        }
    }

    while (true) {
        Menu::clearScreen();
        std::cout << "\n┌────────────────────────────────┐\n";
        std::cout << "│            购物车              │\n";
        std::cout << "├────────────────────────────────┤\n";
        
        // 显示总金额
        double total = cart.calculateTotal();
        std::cout << "总金额: ￥" << std::fixed << std::setprecision(2) << total << "\n\n";

        std::cout << "1. 查看购物车\n";
        std::cout << "2. 修改商品数量\n";
        std::cout << "3. 删除商品\n";
        std::cout << "4. 清空购物车\n";
        std::cout << "5. 结算\n";
        std::cout << "0. 返回\n";
        std::cout << "└────────────────────────────────┘\n";
        std::cout << "请选择操作 [0-5]: ";

        int choice = Menu::getChoice(0, 5);
        if (choice == 0) break;

        try {
            switch (choice) {
                case 1:
                    displayCart(cart);
                    waitForKey();  // 只在这里调用一次
                    break;
                    
                case 2:
                    handleUpdateCartItem(cart);
                    // 更新数据库
                    for (const auto& item : cart.getItems()) {
                        db->saveCartItem(username, item.getProduct()->getId(), 
                                       item.getQuantity());
                    }
                    break;
                    
                case 3:
                    handleRemoveCartItem(cart);
                    break;
                    
                case 4:
                    if (confirmAction("确认清空购物车？")) {
                        try {
                            // 先清空数据库中的购物车记录
                            if (db->removeCartItem(username, -1)) {
                                // 成功清空数据库后再清空内存中的购物车
                                cart.clear();
                                std::cout << "购物车已清空\n";
                            } else {
                                std::cout << "清空购物车失败\n";
                            }
                        } catch (const std::exception& e) {
                            std::cerr << "清空购物车失败: " << e.what() << "\n";
                        }
                    }
                    break;
                    
                case 5:
                    if (!cart.getItems().empty()) {
                        handleCheckout(cart, username);
                    } else {
                        std::cout << "购物车为空，无法结算\n";
                    }
                    break;
            }
            
        } catch (const std::exception& e) {
            std::cerr << "操作失败: " << e.what() << "\n";
            waitForKey();
        }
    }
}