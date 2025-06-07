#include "cart.h"
#include <sstream>
#include <iomanip>
#include <stdexcept>

std::string CartItem::serialize() const {
    std::ostringstream oss;
    oss << productId << ";" << productName << ";" << productType << ";"
        << std::fixed << std::setprecision(2) << originalPrice << ";"
        << std::fixed << std::setprecision(2) << currentPrice << ";"
        << quantity << ";" << merchantName << ";"
        << std::fixed << std::setprecision(2) << discount;
    return oss.str();
}

CartItem CartItem::deserialize(const std::string& data) {
    std::istringstream iss(data);
    std::string idStr, name, type, origPriceStr, currPriceStr, qtyStr, merchant, discountStr;

    if (std::getline(iss, idStr, ';') &&
        std::getline(iss, name, ';') &&
        std::getline(iss, type, ';') &&
        std::getline(iss, origPriceStr, ';') &&
        std::getline(iss, currPriceStr, ';') &&
        std::getline(iss, qtyStr, ';') &&
        std::getline(iss, merchant, ';') &&
        std::getline(iss, discountStr)) {

        try {
            return CartItem(std::stoi(idStr), name, type,
                std::stod(origPriceStr), std::stod(currPriceStr),
                std::stoi(qtyStr), merchant, std::stod(discountStr));
        }
        catch (const std::exception& e) {
            // 转换失败，返回空的CartItem
            return CartItem();
        }
    }

    return CartItem(); // 返回空的CartItem
}

bool Cart::addItem(const CartItem& item) {
    if (item.productId <= 0 || item.quantity <= 0) {
        return false;
    }

    auto it = items.find(item.productId);
    if (it != items.end()) {
        // 商品已存在，增加数量
        it->second.quantity += item.quantity;
    }
    else {
        // 新商品，直接添加
        items[item.productId] = item;
    }

    return true;
}

bool Cart::updateItemQuantity(int productId, int quantity) {
    auto it = items.find(productId);
    if (it == items.end()) {
        return false;
    }

    if (quantity <= 0) {
        items.erase(it);
    }
    else {
        it->second.quantity = quantity;
    }

    return true;
}

bool Cart::removeItem(int productId) {
    auto it = items.find(productId);
    if (it == items.end()) {
        return false;
    }

    items.erase(it);
    return true;
}

void Cart::clear() {
    items.clear();
}

std::vector<CartItem> Cart::getItems() const {
    std::vector<CartItem> result;
    for (const auto& pair : items) {
        result.push_back(pair.second);
    }
    return result;
}

int Cart::getTotalItemCount() const {
    int total = 0;
    for (const auto& pair : items) {
        total += pair.second.quantity;
    }
    return total;
}

double Cart::getTotalPrice() const {
    double total = 0.0;
    for (const auto& pair : items) {
        total += pair.second.getTotalPrice();
    }
    return total;
}

bool Cart::isEmpty() const {
    return items.empty();
}

std::string Cart::serialize() const {
    std::ostringstream oss;
    oss << username << "|" << items.size();

    for (const auto& pair : items) {
        oss << "|" << pair.second.serialize();
    }

    return oss.str();
}

Cart Cart::deserialize(const std::string& data) {
    std::istringstream iss(data);
    std::string username, countStr;

    if (!std::getline(iss, username, '|') || !std::getline(iss, countStr, '|')) {
        return Cart(); // 返回空购物车
    }

    Cart cart(username);

    try {
        int count = std::stoi(countStr);

        for (int i = 0; i < count; ++i) {
            std::string itemData;
            if (std::getline(iss, itemData, '|')) {
                CartItem item = CartItem::deserialize(itemData);
                if (item.productId > 0) {
                    cart.addItem(item);
                }
            }
        }
    }
    catch (const std::exception& e) {
        // 解析失败，返回空购物车
        return Cart(username);
    }

    return cart;
}