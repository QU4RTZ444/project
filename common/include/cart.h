#ifndef CART_H
#define CART_H

#include <string>
#include <vector>
#include <map>

// 购物车中的商品项
struct CartItem {
    int productId;           // 商品ID
    std::string productName; // 商品名称
    std::string productType; // 商品类型
    double originalPrice;    // 商品原价
    double currentPrice;     // 商品现价（考虑折扣）
    int quantity;           // 购买数量
    std::string merchantName; // 商家名称
    double discount;        // 折扣

    CartItem() : productId(0), originalPrice(0.0), currentPrice(0.0), quantity(0), discount(1.0) {}

    CartItem(int id, const std::string& name, const std::string& type,
        double origPrice, double currPrice, int qty,
        const std::string& merchant, double disc)
        : productId(id), productName(name), productType(type),
        originalPrice(origPrice), currentPrice(currPrice), quantity(qty),
        merchantName(merchant), discount(disc) {}

    // 计算该项目的总价
    double getTotalPrice() const {
        return currentPrice * quantity;
    }

    // 序列化为字符串
    std::string serialize() const;

    // 从字符串反序列化
    static CartItem deserialize(const std::string& data);
};

// 购物车类
class Cart {
private:
    std::string username;
    std::map<int, CartItem> items; // productId -> CartItem

public:
    // 默认构造函数
    Cart() : username("") {}

    // 带用户名的构造函数
    Cart(const std::string& user) : username(user) {}

    // 拷贝构造函数
    Cart(const Cart& other) : username(other.username), items(other.items) {}

    // 赋值操作符
    Cart& operator=(const Cart& other) {
        if (this != &other) {
            username = other.username;
            items = other.items;
        }
        return *this;
    }

    // 设置用户名（用于默认构造后设置）
    void setUsername(const std::string& user) {
        username = user;
    }

    // 添加商品到购物车
    bool addItem(const CartItem& item);

    // 更新商品数量
    bool updateItemQuantity(int productId, int quantity);

    // 移除商品
    bool removeItem(int productId);

    // 清空购物车
    void clear();

    // 获取购物车中的所有商品
    std::vector<CartItem> getItems() const;

    // 获取购物车中商品的总数量
    int getTotalItemCount() const;

    // 获取购物车总价
    double getTotalPrice() const;

    // 检查购物车是否为空
    bool isEmpty() const;

    // 获取用户名
    std::string getUsername() const { return username; }

    // 序列化购物车
    std::string serialize() const;

    // 反序列化购物车
    static Cart deserialize(const std::string& data);
};

#endif