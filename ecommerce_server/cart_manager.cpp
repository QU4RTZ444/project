#include "cart_manager.h"
#include <fstream>
#include <iostream>

CartManager::CartManager(const std::string& filename) : filename(filename) {
    loadCarts();
}

CartManager::~CartManager() {
    saveCarts();
}

bool CartManager::addItemToCart(const std::string& username, const CartItem& item) {
    std::lock_guard<std::mutex> lock(cartsMutex);

    auto it = userCarts.find(username);
    if (it == userCarts.end()) {
        // 创建新的购物车
        userCarts.emplace(username, Cart(username));
        it = userCarts.find(username);
    }

    bool result = it->second.addItem(item);
    if (result) {
        saveCarts();
        std::cout << "用户 [" << username << "] 添加商品到购物车: " << item.productName
            << " x" << item.quantity << std::endl;
    }

    return result;
}

bool CartManager::updateCartItem(const std::string& username, int productId, int quantity) {
    std::lock_guard<std::mutex> lock(cartsMutex);

    auto it = userCarts.find(username);
    if (it == userCarts.end()) {
        return false;
    }

    bool result = it->second.updateItemQuantity(productId, quantity);
    if (result) {
        saveCarts();
        std::cout << "用户 [" << username << "] 更新购物车商品数量: 商品ID=" << productId
            << ", 新数量=" << quantity << std::endl;
    }

    return result;
}

bool CartManager::removeItemFromCart(const std::string& username, int productId) {
    std::lock_guard<std::mutex> lock(cartsMutex);

    auto it = userCarts.find(username);
    if (it == userCarts.end()) {
        return false;
    }

    bool result = it->second.removeItem(productId);
    if (result) {
        saveCarts();
        std::cout << "用户 [" << username << "] 从购物车移除商品: 商品ID=" << productId << std::endl;
    }

    return result;
}

bool CartManager::clearUserCart(const std::string& username) {
    std::lock_guard<std::mutex> lock(cartsMutex);

    auto it = userCarts.find(username);
    if (it == userCarts.end()) {
        return false;
    }

    it->second.clear();
    saveCarts();
    std::cout << "用户 [" << username << "] 清空购物车" << std::endl;

    return true;
}

Cart CartManager::getUserCart(const std::string& username) const {
    std::lock_guard<std::mutex> lock(cartsMutex);

    auto it = userCarts.find(username);
    if (it != userCarts.end()) {
        return it->second;
    }

    return Cart(username); // 返回空购物车
}

std::vector<CartItem> CartManager::getUserCartItems(const std::string& username) const {
    std::lock_guard<std::mutex> lock(cartsMutex);

    auto it = userCarts.find(username);
    if (it != userCarts.end()) {
        return it->second.getItems();
    }

    return std::vector<CartItem>(); // 返回空列表
}

double CartManager::getUserCartTotalPrice(const std::string& username) const {
    std::lock_guard<std::mutex> lock(cartsMutex);

    auto it = userCarts.find(username);
    if (it != userCarts.end()) {
        return it->second.getTotalPrice();
    }

    return 0.0;
}

int CartManager::getUserCartItemCount(const std::string& username) const {
    std::lock_guard<std::mutex> lock(cartsMutex);

    auto it = userCarts.find(username);
    if (it != userCarts.end()) {
        return it->second.getTotalItemCount();
    }

    return 0;
}

void CartManager::loadCarts() {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "购物车文件不存在，将在首次使用时创建: " << filename << std::endl;
        return;
    }

    userCarts.clear();

    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty()) {
            try {
                Cart cart = Cart::deserialize(line);
                std::string username = cart.getUsername();
                if (!username.empty()) {
                    userCarts.emplace(username, std::move(cart));
                }
            }
            catch (const std::exception& e) {
                std::cerr << "解析购物车数据时出错: " << e.what() << std::endl;
            }
        }
    }

    file.close();
    std::cout << "成功加载 " << userCarts.size() << " 个用户的购物车" << std::endl;
}

void CartManager::saveCarts() {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "无法打开购物车文件进行写入: " << filename << std::endl;
        return;
    }

    for (const auto& pair : userCarts) {
        if (!pair.second.isEmpty()) { // 只保存非空购物车
            file << pair.second.serialize() << std::endl;
        }
    }

    file.close();
}