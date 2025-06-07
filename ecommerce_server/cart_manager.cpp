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
        // �����µĹ��ﳵ
        userCarts.emplace(username, Cart(username));
        it = userCarts.find(username);
    }

    bool result = it->second.addItem(item);
    if (result) {
        saveCarts();
        std::cout << "�û� [" << username << "] �����Ʒ�����ﳵ: " << item.productName
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
        std::cout << "�û� [" << username << "] ���¹��ﳵ��Ʒ����: ��ƷID=" << productId
            << ", ������=" << quantity << std::endl;
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
        std::cout << "�û� [" << username << "] �ӹ��ﳵ�Ƴ���Ʒ: ��ƷID=" << productId << std::endl;
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
    std::cout << "�û� [" << username << "] ��չ��ﳵ" << std::endl;

    return true;
}

Cart CartManager::getUserCart(const std::string& username) const {
    std::lock_guard<std::mutex> lock(cartsMutex);

    auto it = userCarts.find(username);
    if (it != userCarts.end()) {
        return it->second;
    }

    return Cart(username); // ���ؿչ��ﳵ
}

std::vector<CartItem> CartManager::getUserCartItems(const std::string& username) const {
    std::lock_guard<std::mutex> lock(cartsMutex);

    auto it = userCarts.find(username);
    if (it != userCarts.end()) {
        return it->second.getItems();
    }

    return std::vector<CartItem>(); // ���ؿ��б�
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
        std::cout << "���ﳵ�ļ������ڣ������״�ʹ��ʱ����: " << filename << std::endl;
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
                std::cerr << "�������ﳵ����ʱ����: " << e.what() << std::endl;
            }
        }
    }

    file.close();
    std::cout << "�ɹ����� " << userCarts.size() << " ���û��Ĺ��ﳵ" << std::endl;
}

void CartManager::saveCarts() {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "�޷��򿪹��ﳵ�ļ�����д��: " << filename << std::endl;
        return;
    }

    for (const auto& pair : userCarts) {
        if (!pair.second.isEmpty()) { // ֻ����ǿչ��ﳵ
            file << pair.second.serialize() << std::endl;
        }
    }

    file.close();
}