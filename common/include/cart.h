#ifndef CART_H
#define CART_H

#include <string>
#include <vector>
#include <map>

// ���ﳵ�е���Ʒ��
struct CartItem {
    int productId;           // ��ƷID
    std::string productName; // ��Ʒ����
    std::string productType; // ��Ʒ����
    double originalPrice;    // ��Ʒԭ��
    double currentPrice;     // ��Ʒ�ּۣ������ۿۣ�
    int quantity;           // ��������
    std::string merchantName; // �̼�����
    double discount;        // �ۿ�

    CartItem() : productId(0), originalPrice(0.0), currentPrice(0.0), quantity(0), discount(1.0) {}

    CartItem(int id, const std::string& name, const std::string& type,
        double origPrice, double currPrice, int qty,
        const std::string& merchant, double disc)
        : productId(id), productName(name), productType(type),
        originalPrice(origPrice), currentPrice(currPrice), quantity(qty),
        merchantName(merchant), discount(disc) {}

    // �������Ŀ���ܼ�
    double getTotalPrice() const {
        return currentPrice * quantity;
    }

    // ���л�Ϊ�ַ���
    std::string serialize() const;

    // ���ַ��������л�
    static CartItem deserialize(const std::string& data);
};

// ���ﳵ��
class Cart {
private:
    std::string username;
    std::map<int, CartItem> items; // productId -> CartItem

public:
    // Ĭ�Ϲ��캯��
    Cart() : username("") {}

    // ���û����Ĺ��캯��
    Cart(const std::string& user) : username(user) {}

    // �������캯��
    Cart(const Cart& other) : username(other.username), items(other.items) {}

    // ��ֵ������
    Cart& operator=(const Cart& other) {
        if (this != &other) {
            username = other.username;
            items = other.items;
        }
        return *this;
    }

    // �����û���������Ĭ�Ϲ�������ã�
    void setUsername(const std::string& user) {
        username = user;
    }

    // �����Ʒ�����ﳵ
    bool addItem(const CartItem& item);

    // ������Ʒ����
    bool updateItemQuantity(int productId, int quantity);

    // �Ƴ���Ʒ
    bool removeItem(int productId);

    // ��չ��ﳵ
    void clear();

    // ��ȡ���ﳵ�е�������Ʒ
    std::vector<CartItem> getItems() const;

    // ��ȡ���ﳵ����Ʒ��������
    int getTotalItemCount() const;

    // ��ȡ���ﳵ�ܼ�
    double getTotalPrice() const;

    // ��鹺�ﳵ�Ƿ�Ϊ��
    bool isEmpty() const;

    // ��ȡ�û���
    std::string getUsername() const { return username; }

    // ���л����ﳵ
    std::string serialize() const;

    // �����л����ﳵ
    static Cart deserialize(const std::string& data);
};

#endif