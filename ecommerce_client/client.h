#ifndef CLIENT_H
#define CLIENT_H

#define NOMINMAX
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <thread>
#include <limits>
#include <vector>
#include "message.h"

#pragma comment(lib, "ws2_32.lib")

struct ProductInfo {
    int id;
    std::string name;
    double originalPrice;    // 原价
    double price;           // 现价（考虑折扣）
    int stock;
    std::string merchant;
    std::string productType; // 商品种类
    double discount;        // 折扣
    std::string description;
};

// 添加购物车商品信息结构体
struct CartItemInfo {
    int id;
    std::string name;
    std::string type;
    double originalPrice;
    double currentPrice;
    int quantity;
    std::string merchant;
    double discount;

    double getTotalPrice() const {
        return currentPrice * quantity;
    }
};

// 订单项目信息结构体
struct OrderItemInfo {
    int productId;
    std::string productName;
    std::string productType;
    double originalPrice;
    double currentPrice;
    int quantity;
    std::string merchantName;
    double discount;

    double getTotalPrice() const {
        return currentPrice * quantity;
    }
};

// 订单信息结构体
struct OrderInfo {
    int orderId;
    std::string orderTime;
    double totalAmount;
    std::string status;
    std::vector<OrderItemInfo> items;
};

class Client {
private:
    SOCKET clientSocket;
    std::thread receiveThread;
    bool connected;

    // 用户信息
    std::string currentUser;
    std::string userType;
    double userBalance;

    // 商品浏览相关
    std::vector<ProductInfo> currentProducts;
    int currentPage;
    int totalPages;
    size_t totalCount;
    bool waitingForResponse;

    // 购物车相关
    std::vector<CartItemInfo> currentCartItems;
    double cartTotalPrice;
    int cartTotalCount;

    // 当前订单信息
    std::vector<OrderInfo> currentOrders;

    // 接收消息的线程函数
    void receiveMessages();

    // 处理接收到的消息
    void handleMessage(const NetworkMessage& message);

public:
    Client();
    ~Client();

    // 连接到服务器
    bool connectToServer(const std::string& serverIP, int port);

    // 断开连接
    void disconnect();

    // 检查连接状态
    bool isConnected() const;

    // 发送消息
    bool sendMessage(const NetworkMessage& message);

    // 处理用户操作
    void handleRegister();
    void handleLogin();
    void handleLogout();
    void handleAccountManagement();
    void handleChangePassword();
    void handleProductBrowse();
    void handleProductList();
    void handleProductSearch();
    void handleProductDetail(int productId);

    // 商家商品管理
    void handleMerchantProductManagement();
    void handleMerchantAddProduct();
    void handleMerchantProductList();
    void handleMerchantModifyProduct();
    void handleMerchantSetDiscount();

    // 购物车管理
    void handleAddToCart(int productId, int quantity);
    void handleCartManagement();
    void handleViewCart();
    void handleUpdateCartItem();
    void handleRemoveCartItem();
    void handleClearCart();

    // 购物车结算
    void handleCheckout();
    void handleConfirmOrder();

    // 订单管理
    void handleOrderManagement();
    void handleViewOrders();
    void handleOrderDetail(int orderId);

    // 运行客户端主循环
    void run();
};

#endif