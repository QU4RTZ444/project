#ifndef CART_MANAGER_H
#define CART_MANAGER_H

#include "cart.h"
#include <map>
#include <mutex>
#include <string>

class CartManager {
private:
    std::map<std::string, Cart> userCarts; // username -> Cart
    mutable std::mutex cartsMutex;
    std::string filename;

    void loadCarts();
    void saveCarts();

public:
    CartManager(const std::string& filename);
    ~CartManager();

    // �����Ʒ���û����ﳵ
    bool addItemToCart(const std::string& username, const CartItem& item);

    // ���¹��ﳵ����Ʒ����
    bool updateCartItem(const std::string& username, int productId, int quantity);

    // �ӹ��ﳵ�Ƴ���Ʒ
    bool removeItemFromCart(const std::string& username, int productId);

    // ����û����ﳵ
    bool clearUserCart(const std::string& username);

    // ��ȡ�û����ﳵ
    Cart getUserCart(const std::string& username) const;

    // ��ȡ�û����ﳵ�е���Ʒ�б�
    std::vector<CartItem> getUserCartItems(const std::string& username) const;

    // ��ȡ�û����ﳵ�ܼ�
    double getUserCartTotalPrice(const std::string& username) const;

    // ��ȡ�û����ﳵ��Ʒ����
    int getUserCartItemCount(const std::string& username) const;
};

#endif