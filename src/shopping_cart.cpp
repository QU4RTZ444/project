#include "include.h"

bool ShoppingCart::addItem(const std::shared_ptr<Product>& product, int quantity) {
    if (!product || quantity <= 0) {
        return false;
    }

    // 检查是否已达到购物车最大容量
    if (items.size() >= MAX_CART_ITEMS && !containsProduct(product->getId())) {
        throw std::runtime_error("购物车已满");
    }

    // 检查商品库存
    if (quantity > product->getQuantity()) {
        throw std::runtime_error("商品库存不足");
    }

    // 如果商品已在购物车中，更新数量
    for (auto& item : items) {
        if (item.getProduct()->getId() == product->getId()) {
            int newQuantity = item.getQuantity() + quantity;
            if (newQuantity > product->getQuantity()) {
                throw std::runtime_error("商品库存不足");
            }
            item.setQuantity(newQuantity);
            return true;
        }
    }

    // 添加新商品
    items.emplace_back(product, quantity);
    return true;
}

bool ShoppingCart::removeItem(int productId) {
    auto it = std::find_if(items.begin(), items.end(),
        [productId](const CartItem& item) {
            return item.getProduct()->getId() == productId;
        });

    if (it != items.end()) {
        items.erase(it);
        return true;
    }
    return false;
}

bool ShoppingCart::updateQuantity(int productId, int newQuantity) {
    if (newQuantity <= 0) {
        return false;
    }

    for (auto& item : items) {
        if (item.getProduct()->getId() == productId) {
            if (newQuantity > item.getProduct()->getQuantity()) {
                throw std::runtime_error("商品库存不足");
            }
            item.setQuantity(newQuantity);
            return true;
        }
    }
    return false;
}

void ShoppingCart::clear() {
    items.clear();
}

double ShoppingCart::calculateTotal() const {
    double total = 0.0;
    for (const auto& item : items) {
        auto product = item.getProduct();
        // 使用 getPrice() 获取折扣后的价格
        double discountedPrice = product->getPrice();
        total += discountedPrice * item.getQuantity();
    }
    return total;
}

bool ShoppingCart::containsProduct(int productId) const {
    return std::any_of(items.begin(), items.end(),
        [productId](const CartItem& item) {
            return item.getProduct()->getId() == productId;
        });
}