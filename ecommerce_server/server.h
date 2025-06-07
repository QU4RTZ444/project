#ifndef SERVER_H
#define SERVER_H

#define _CRT_SECURE_NO_WARNINGS

#include <winsock2.h>
#include <ws2tcpip.h>
#include <vector>
#include <thread>
#include <mutex>
#include <map>
#include <unordered_map>
#include "message.h"
#include "user_manager.h"
#include "product_manager.h"
#include "cart_manager.h"  // 确保包含购物车管理器

#pragma comment(lib, "ws2_32.lib")

class Server {
private:
    SOCKET serverSocket;
    std::vector<SOCKET> clientSockets;
    std::map<SOCKET, std::string> clientInfo;
    std::unordered_map<SOCKET, User*> loggedInUsers;
    std::mutex clientsMutex;
    bool running;
    int port;

    UserManager userManager;
    ProductManager productManager;
    CartManager cartManager; // 购物车管理器

    // 处理客户端连接的线程函数
    void handleClient(SOCKET clientSocket);

    // 处理接收到的消息
    void handleMessage(SOCKET clientSocket, const NetworkMessage& message);

    // 处理具体的请求
    void handleRegisterRequest(SOCKET clientSocket, const std::string& data);
    void handleLoginRequest(SOCKET clientSocket, const std::string& data);
    void handleLogoutRequest(SOCKET clientSocket);
    void handleChangePasswordRequest(SOCKET clientSocket, const std::string& data);
    void handleProductListRequest(SOCKET clientSocket, const std::string& data);
    void handleProductSearchRequest(SOCKET clientSocket, const std::string& data);
    void handleProductDetailRequest(SOCKET clientSocket, const std::string& data);

    // 商家商品管理
    void handleMerchantAddProductRequest(SOCKET clientSocket, const std::string& data);
    void handleMerchantModifyProductRequest(SOCKET clientSocket, const std::string& data);
    void handleMerchantProductListRequest(SOCKET clientSocket, const std::string& data);
    void handleMerchantSetDiscountRequest(SOCKET clientSocket, const std::string& data);

    // 购物车管理
    void handleCartAddItemRequest(SOCKET clientSocket, const std::string& data);
    void handleCartViewRequest(SOCKET clientSocket, const std::string& data);
    void handleCartUpdateItemRequest(SOCKET clientSocket, const std::string& data);
    void handleCartRemoveItemRequest(SOCKET clientSocket, const std::string& data);
    void handleCartClearRequest(SOCKET clientSocket, const std::string& data);

    // 订单结算
    void handleOrderCheckoutRequest(SOCKET clientSocket, const std::string& data);

    // 发送消息给客户端
    bool sendMessage(SOCKET clientSocket, const NetworkMessage& message);

    // 广播消息给所有客户端
    void broadcastMessage(const NetworkMessage& message);

    // 订单管理
    void handleOrderListRequest(SOCKET clientSocket, const std::string& data);

public:
    Server(int port = 8080);
    ~Server();

    // 启动服务器
    bool start();

    // 停止服务器
    void stop();

    // 运行服务器主循环
    void run();
};

#endif