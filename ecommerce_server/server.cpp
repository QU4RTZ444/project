#include "server.h"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <iomanip>

Server::Server(int port) : port(port), running(false), serverSocket(INVALID_SOCKET),
userManager("users.txt"), productManager("products.txt"),
cartManager("carts.txt") {
    // 初始化Winsock
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "WSAStartup失败: " << result << std::endl;
        throw std::runtime_error("Winsock初始化失败");
    }
}

Server::~Server() {
    stop();
    WSACleanup();
}

bool Server::start() {
    // 创建socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "创建socket失败: " << WSAGetLastError() << std::endl;
        return false;
    }

    // 设置地址
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    // 绑定socket
    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "绑定失败: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        return false;
    }

    // 监听连接
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "监听失败: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        return false;
    }

    running = true;
    std::cout << "服务器启动成功，监听端口: " << port << std::endl;
    return true;
}

void Server::run() {
    while (running) {
        sockaddr_in clientAddr;
        int clientAddrSize = sizeof(clientAddr);

        SOCKET clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientAddrSize);
        if (clientSocket != INVALID_SOCKET) {
            std::lock_guard<std::mutex> lock(clientsMutex);
            clientSockets.push_back(clientSocket);

            char clientIP[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, INET_ADDRSTRLEN);
            clientInfo[clientSocket] = std::string(clientIP) + ":" + std::to_string(ntohs(clientAddr.sin_port));

            std::cout << "新客户端连接: " << clientInfo[clientSocket] << std::endl;

            // 为每个客户端创建处理线程
            std::thread clientThread(&Server::handleClient, this, clientSocket);
            clientThread.detach();
        }
    }
}

void Server::handleClient(SOCKET clientSocket) {
    char buffer[4096];

    while (running) {
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);

        if (bytesReceived <= 0) {
            // 客户端断开连接
            std::cout << "客户端断开连接: " << clientInfo[clientSocket] << std::endl;
            break;
        }

        // 解析消息
        std::vector<char> messageBuffer(buffer, buffer + bytesReceived);
        NetworkMessage message = NetworkMessage::deserialize(messageBuffer);

        handleMessage(clientSocket, message);
    }

    // 清理客户端连接
    std::lock_guard<std::mutex> lock(clientsMutex);
    auto it = std::find(clientSockets.begin(), clientSockets.end(), clientSocket);
    if (it != clientSockets.end()) {
        clientSockets.erase(it);
    }
    clientInfo.erase(clientSocket);

    // 清理已登录用户记录
    auto loginIt = loggedInUsers.find(clientSocket);
    if (loginIt != loggedInUsers.end()) {
        std::cout << "用户 " << loginIt->second->getUsername() << " 连接断开" << std::endl;
        loggedInUsers.erase(loginIt);
    }

    closesocket(clientSocket);
}

void Server::handleMessage(SOCKET clientSocket, const NetworkMessage& message) {
    std::cout << "收到消息 - 类型: " << static_cast<int>(message.type)
        << ", 数据: " << message.data << std::endl;

    NetworkMessage response;

    switch (message.type) {
    case MessageType::CONNECT_REQUEST:
        response = NetworkMessage(MessageType::CONNECT_RESPONSE, "连接成功");
        sendMessage(clientSocket, response);
        break;

    case MessageType::REGISTER_REQUEST:
        handleRegisterRequest(clientSocket, message.data);
        break;

    case MessageType::LOGIN_REQUEST:
        handleLoginRequest(clientSocket, message.data);
        break;

    case MessageType::LOGOUT_REQUEST:
        handleLogoutRequest(clientSocket);
        break;

    case MessageType::CHANGE_PASSWORD_REQUEST:
        handleChangePasswordRequest(clientSocket, message.data);
        break;

    case MessageType::PRODUCT_LIST_REQUEST:
        handleProductListRequest(clientSocket, message.data);
        break;

    case MessageType::PRODUCT_SEARCH_REQUEST:
        handleProductSearchRequest(clientSocket, message.data);
        break;

    case MessageType::PRODUCT_DETAIL_REQUEST:
        handleProductDetailRequest(clientSocket, message.data);
        break;

    case MessageType::MERCHANT_ADD_PRODUCT_REQUEST:
        handleMerchantAddProductRequest(clientSocket, message.data);
        break;

    case MessageType::MERCHANT_MODIFY_PRODUCT_REQUEST:
        handleMerchantModifyProductRequest(clientSocket, message.data);
        break;

    case MessageType::MERCHANT_PRODUCT_LIST_REQUEST:
        handleMerchantProductListRequest(clientSocket, message.data);
        break;

    case MessageType::MERCHANT_SET_DISCOUNT_REQUEST:
        handleMerchantSetDiscountRequest(clientSocket, message.data);
        break;

        // 购物车相关消息处理 - 这里是缺失的部分
    case MessageType::CART_ADD_ITEM_REQUEST:
        handleCartAddItemRequest(clientSocket, message.data);
        break;

    case MessageType::CART_VIEW_REQUEST:
        handleCartViewRequest(clientSocket, message.data);
        break;

    case MessageType::CART_UPDATE_ITEM_REQUEST:
        handleCartUpdateItemRequest(clientSocket, message.data);
        break;

    case MessageType::CART_REMOVE_ITEM_REQUEST:
        handleCartRemoveItemRequest(clientSocket, message.data);
        break;

    case MessageType::CART_CLEAR_REQUEST:
        handleCartClearRequest(clientSocket, message.data);
        break;

    case MessageType::ORDER_CHECKOUT_REQUEST:
        handleOrderCheckoutRequest(clientSocket, message.data);
        break;

    case MessageType::ORDER_LIST_REQUEST:
        handleOrderListRequest(clientSocket, message.data);
        break;

    case MessageType::DISCONNECT:
        std::cout << "客户端请求断开连接" << std::endl;
        break;

    default:
        std::cout << "未处理的消息类型: " << static_cast<int>(message.type) << std::endl;
        // 简单的回显服务
        response = NetworkMessage(MessageType::SUCCESS_RESPONSE, "服务器收到: " + message.data);
        sendMessage(clientSocket, response);
        break;
    }
}

void Server::handleRegisterRequest(SOCKET clientSocket, const std::string& data) {
    std::istringstream iss(data);
    std::string username, password, userTypeStr;

    if (std::getline(iss, username, '|') &&
        std::getline(iss, password, '|') &&
        std::getline(iss, userTypeStr)) {

        UserType userType = (userTypeStr == "1") ? UserType::CONSUMER : UserType::MERCHANT;

        if (userManager.registerUser(username, password, userType)) {
            std::string response = "SUCCESS|用户注册成功";
            sendMessage(clientSocket, NetworkMessage(MessageType::REGISTER_RESPONSE, response));
        }
        else {
            std::string response = "ERROR|用户名已存在或注册失败";
            sendMessage(clientSocket, NetworkMessage(MessageType::REGISTER_RESPONSE, response));
        }
    }
    else {
        std::string response = "ERROR|注册数据格式错误";
        sendMessage(clientSocket, NetworkMessage(MessageType::REGISTER_RESPONSE, response));
    }
}

void Server::handleLoginRequest(SOCKET clientSocket, const std::string& data) {
    std::istringstream iss(data);
    std::string username, password;

    if (std::getline(iss, username, '|') && std::getline(iss, password)) {
        User* user = userManager.authenticateUser(username, password);
        if (user) {
            // 如果是商家登录，读取订单文件计算总收入作为余额
            if (user->getUserType() == UserType::MERCHANT) {
                std::string merchantOrderFile = "orders_" + username + ".txt";
                std::ifstream file(merchantOrderFile);

                if (file.is_open()) {
                    double totalEarnings = 0.0;
                    int orderCount = 0;
                    std::string line;

                    while (std::getline(file, line)) {
                        if (line.find("收入:") == 0) {
                            try {
                                std::string amountStr = line.substr(3); // 去掉"收入:"

                                // 调试输出
                                std::cout << "解析收入行: [" << line << "]" << std::endl;
                                std::cout << "提取金额字符串: [" << amountStr << "]" << std::endl;

                                double amount = std::stod(amountStr);
                                totalEarnings += amount;
                                orderCount++;

                                std::cout << "解析成功，金额: " << amount << std::endl;
                            }
                            catch (const std::exception& e) {
                                std::cerr << "解析商家收入记录时出错: " << e.what() << std::endl;
                                std::cerr << "问题行内容: [" << line << "]" << std::endl;

                                // 尝试手动解析
                                size_t colonPos = line.find(':');
                                if (colonPos != std::string::npos) {
                                    std::string manualAmountStr = line.substr(colonPos + 1);
                                    std::cout << "手动提取: [" << manualAmountStr << "]" << std::endl;
                                    try {
                                        double amount = std::stod(manualAmountStr);
                                        totalEarnings += amount;
                                        orderCount++;
                                        std::cout << "手动解析成功，金额: " << amount << std::endl;
                                    }
                                    catch (const std::exception& e2) {
                                        std::cerr << "手动解析也失败: " << e2.what() << std::endl;
                                    }
                                }
                            }
                        }
                    }
                    file.close();

                    // 将累计收入设置为商家余额
                    user->setBalance(totalEarnings);

                    std::cout << "商家 [" << username << "] 登录时计算余额:" << std::endl;
                    std::cout << "  订单数量: " << orderCount << std::endl;
                    std::cout << "  累计收入: " << totalEarnings << " 元" << std::endl;

                    // 保存用户数据
                    userManager.saveUsers();
                }
                else {
                    std::cout << "商家 [" << username << "] 没有订单记录，余额为0" << std::endl;
                    user->setBalance(0.0);
                }
            }

            // 记录已登录用户
            {
                std::lock_guard<std::mutex> lock(clientsMutex);
                loggedInUsers[clientSocket] = user;
            }

            std::string userTypeStr = (user->getUserType() == UserType::CONSUMER) ? "消费者" : "商家";
            std::string response = "SUCCESS|登录成功|" + userTypeStr + "|" + std::to_string(user->getBalance());
            sendMessage(clientSocket, NetworkMessage(MessageType::LOGIN_RESPONSE, response));

            std::cout << "用户 [" << username << "] 登录成功，当前余额: " << user->getBalance() << " 元" << std::endl;
        }
        else {
            std::string response = "ERROR|用户名或密码错误";
            sendMessage(clientSocket, NetworkMessage(MessageType::LOGIN_RESPONSE, response));
        }
    }
    else {
        std::string response = "ERROR|登录数据格式错误";
        sendMessage(clientSocket, NetworkMessage(MessageType::LOGIN_RESPONSE, response));
    }
}

void Server::handleLogoutRequest(SOCKET clientSocket) {
    std::lock_guard<std::mutex> lock(clientsMutex);
    auto it = loggedInUsers.find(clientSocket);
    if (it != loggedInUsers.end()) {
        std::string username = it->second->getUsername();  // 现在可以正确访问了
        loggedInUsers.erase(it);

        std::string response = "SUCCESS|用户 " + username + " 已成功登出";
        sendMessage(clientSocket, NetworkMessage(MessageType::LOGOUT_RESPONSE, response));
        std::cout << "用户 " << username << " 已登出" << std::endl;
    }
    else {
        std::string response = "ERROR|用户未登录";
        sendMessage(clientSocket, NetworkMessage(MessageType::LOGOUT_RESPONSE, response));
    }
}

void Server::handleChangePasswordRequest(SOCKET clientSocket, const std::string& data) {
    // 检查用户是否已登录
    std::lock_guard<std::mutex> lock(clientsMutex);
    auto it = loggedInUsers.find(clientSocket);
    if (it == loggedInUsers.end()) {
        std::string response = "ERROR|请先登录";
        sendMessage(clientSocket, NetworkMessage(MessageType::CHANGE_PASSWORD_RESPONSE, response));
        return;
    }

    std::string username = it->second->getUsername();

    // 解析数据: oldPassword|newPassword
    std::istringstream iss(data);
    std::string oldPassword, newPassword;

    if (std::getline(iss, oldPassword, '|') && std::getline(iss, newPassword)) {
        if (userManager.changePassword(username, oldPassword, newPassword)) {
            std::string response = "SUCCESS|密码修改成功";
            sendMessage(clientSocket, NetworkMessage(MessageType::CHANGE_PASSWORD_RESPONSE, response));
        }
        else {
            std::string response = "ERROR|旧密码错误或修改失败";
            sendMessage(clientSocket, NetworkMessage(MessageType::CHANGE_PASSWORD_RESPONSE, response));
        }
    }
    else {
        std::string response = "ERROR|密码修改数据格式错误";
        sendMessage(clientSocket, NetworkMessage(MessageType::CHANGE_PASSWORD_RESPONSE, response));
    }
}

void Server::handleProductListRequest(SOCKET clientSocket, const std::string& data) {
    // 解析数据: page|pageSize
    std::istringstream iss(data);
    std::string pageStr, pageSizeStr;

    int page = 1;
    int pageSize = 5;

    if (std::getline(iss, pageStr, '|') && std::getline(iss, pageSizeStr)) {
        page = std::stoi(pageStr);
        pageSize = std::stoi(pageSizeStr);
    }

    std::vector<ProductInfo> products = productManager.getProductsByPage(page, pageSize);
    int totalPages = productManager.getTotalPages(pageSize);
    size_t totalCount = productManager.getProductCount();

    // 构建响应数据: totalPages|totalCount|currentPage|product1|product2|...
    // 每个商品格式: productId;name;originalPrice;currentPrice;stock;merchantName;productType;discount
    std::ostringstream response;
    response << totalPages << "|" << totalCount << "|" << page;

    for (const auto& product : products) {
        response << "|" << product.productId << ";"
            << product.name << ";"
            << product.originalPrice << ";"
            << product.currentPrice << ";"
            << product.stock << ";"
            << product.merchantName << ";"
            << product.productType << ";"
            << product.discount;
    }

    sendMessage(clientSocket, NetworkMessage(MessageType::PRODUCT_LIST_RESPONSE, response.str()));
}

void Server::handleProductSearchRequest(SOCKET clientSocket, const std::string& data) {
    std::vector<ProductInfo> products = productManager.searchProducts(data);

    // 构建响应数据: count|product1|product2|...
    // 每个商品格式: productId;name;originalPrice;currentPrice;stock;merchantName;productType;discount
    std::ostringstream response;
    response << products.size();

    for (const auto& product : products) {
        response << "|" << product.productId << ";"
            << product.name << ";"
            << product.originalPrice << ";"
            << product.currentPrice << ";"
            << product.stock << ";"
            << product.merchantName << ";"
            << product.productType << ";"
            << product.discount;
    }

    sendMessage(clientSocket, NetworkMessage(MessageType::PRODUCT_SEARCH_RESPONSE, response.str()));
}

void Server::handleProductDetailRequest(SOCKET clientSocket, const std::string& data) {
    int productId = std::stoi(data);
    Product* product = productManager.getProductById(productId);

    if (product) {
        // 构建详细信息响应
        std::ostringstream response;
        response << "SUCCESS|" << product->getProductId() << "|"
            << product->getName() << "|"
            << "商品详情" << "|"  // 简化描述
            << product->getOriginalPrice() << "|"
            << product->getPrice() << "|"
            << product->getStock() << "|"
            << product->getMerchantName() << "|"
            << product->getProductType() << "|"
            << product->getDiscount();

        sendMessage(clientSocket, NetworkMessage(MessageType::PRODUCT_DETAIL_RESPONSE, response.str()));
    }
    else {
        std::string response = "ERROR|商品不存在";
        sendMessage(clientSocket, NetworkMessage(MessageType::PRODUCT_DETAIL_RESPONSE, response));
    }
}

void Server::handleMerchantAddProductRequest(SOCKET clientSocket, const std::string& data) {
    // 检查用户是否已登录且为商家
    std::lock_guard<std::mutex> lock(clientsMutex);
    auto it = loggedInUsers.find(clientSocket);
    if (it == loggedInUsers.end()) {
        std::string response = "ERROR|请先登录";
        sendMessage(clientSocket, NetworkMessage(MessageType::MERCHANT_ADD_PRODUCT_RESPONSE, response));
        return;
    }

    User* user = it->second;
    if (user->getUserType() != UserType::MERCHANT) {
        std::string response = "ERROR|只有商家才能添加商品";
        sendMessage(clientSocket, NetworkMessage(MessageType::MERCHANT_ADD_PRODUCT_RESPONSE, response));
        return;
    }

    std::string merchantName = user->getUsername();

    // 解析数据: type|name|price|stock|discount
    std::istringstream iss(data);
    std::string type, name, priceStr, stockStr, discountStr;

    if (std::getline(iss, type, '|') &&
        std::getline(iss, name, '|') &&
        std::getline(iss, priceStr, '|') &&
        std::getline(iss, stockStr, '|') &&
        std::getline(iss, discountStr, '|')) {

        try {
            double price = std::stod(priceStr);
            int stock = std::stoi(stockStr);
            double discount = std::stod(discountStr);

            if (productManager.addProduct(type, name, price, stock, merchantName, discount)) {
                std::string response = "SUCCESS|商品添加成功";
                sendMessage(clientSocket, NetworkMessage(MessageType::MERCHANT_ADD_PRODUCT_RESPONSE, response));
            }
            else {
                std::string response = "ERROR|商品添加失败";
                sendMessage(clientSocket, NetworkMessage(MessageType::MERCHANT_ADD_PRODUCT_RESPONSE, response));
            }
        }
        catch (const std::exception& e) {
            std::string response = "ERROR|数据格式错误: " + std::string(e.what());
            sendMessage(clientSocket, NetworkMessage(MessageType::MERCHANT_ADD_PRODUCT_RESPONSE, response));
        }
    }
    else {
        std::string response = "ERROR|商品数据格式错误";
        sendMessage(clientSocket, NetworkMessage(MessageType::MERCHANT_ADD_PRODUCT_RESPONSE, response));
    }
}

void Server::handleMerchantModifyProductRequest(SOCKET clientSocket, const std::string& data) {
    // 检查用户是否已登录且为商家
    std::lock_guard<std::mutex> lock(clientsMutex);
    auto it = loggedInUsers.find(clientSocket);
    if (it == loggedInUsers.end()) {
        std::string response = "ERROR|请先登录";
        sendMessage(clientSocket, NetworkMessage(MessageType::MERCHANT_MODIFY_PRODUCT_RESPONSE, response));
        return;
    }

    User* user = it->second;
    if (user->getUserType() != UserType::MERCHANT) {
        std::string response = "ERROR|只有商家才能修改商品";
        sendMessage(clientSocket, NetworkMessage(MessageType::MERCHANT_MODIFY_PRODUCT_RESPONSE, response));
        return;
    }

    std::string merchantName = user->getUsername();

    // 解析数据: productId|price|stock|discount
    std::istringstream iss(data);
    std::string idStr, priceStr, stockStr, discountStr;

    if (std::getline(iss, idStr, '|') &&
        std::getline(iss, priceStr, '|') &&
        std::getline(iss, stockStr, '|') &&
        std::getline(iss, discountStr, '|')) {

        try {
            int productId = std::stoi(idStr);

            // 验证商品是否属于该商家（在修改前就验证）
            Product* product = productManager.getProductById(productId);
            if (!product) {
                std::string response = "ERROR|商品不存在";
                sendMessage(clientSocket, NetworkMessage(MessageType::MERCHANT_MODIFY_PRODUCT_RESPONSE, response));
                return;
            }

            if (product->getMerchantName() != merchantName) {
                std::string response = "ERROR|您没有权限修改此商品";
                sendMessage(clientSocket, NetworkMessage(MessageType::MERCHANT_MODIFY_PRODUCT_RESPONSE, response));
                std::cout << "商家 [" << merchantName << "] 尝试修改不属于自己的商品 [" << productId << "] (属于 [" << product->getMerchantName() << "])" << std::endl;
                return;
            }

            // 解析修改参数
            double price = (priceStr == "-1") ? -1 : std::stod(priceStr);
            int stock = (stockStr == "-1") ? -1 : std::stoi(stockStr);
            double discount = (discountStr == "-1") ? -1 : std::stod(discountStr);

            // 记录修改前的信息
            std::cout << "商家 [" << merchantName << "] 正在修改商品 [" << productId << "] " << product->getName() << std::endl;
            std::cout << "修改前: 价格=" << product->getOriginalPrice() << ", 库存=" << product->getStock() << ", 折扣=" << product->getDiscount() << std::endl;

            // 执行修改
            if (productManager.modifyProduct(productId, price, stock, discount)) {
                std::cout << "修改后: 价格=" << product->getOriginalPrice() << ", 库存=" << product->getStock() << ", 折扣=" << product->getDiscount() << std::endl;
                std::string response = "SUCCESS|商品修改成功";
                sendMessage(clientSocket, NetworkMessage(MessageType::MERCHANT_MODIFY_PRODUCT_RESPONSE, response));
            }
            else {
                std::string response = "ERROR|商品修改失败";
                sendMessage(clientSocket, NetworkMessage(MessageType::MERCHANT_MODIFY_PRODUCT_RESPONSE, response));
            }
        }
        catch (const std::exception& e) {
            std::string response = "ERROR|数据格式错误: " + std::string(e.what());
            sendMessage(clientSocket, NetworkMessage(MessageType::MERCHANT_MODIFY_PRODUCT_RESPONSE, response));
        }
    }
    else {
        std::string response = "ERROR|修改数据格式错误";
        sendMessage(clientSocket, NetworkMessage(MessageType::MERCHANT_MODIFY_PRODUCT_RESPONSE, response));
    }
}

void Server::handleMerchantProductListRequest(SOCKET clientSocket, const std::string& data) {
    // 检查用户是否已登录且为商家
    std::lock_guard<std::mutex> lock(clientsMutex);
    auto it = loggedInUsers.find(clientSocket);
    if (it == loggedInUsers.end()) {
        std::string response = "ERROR|请先登录";
        sendMessage(clientSocket, NetworkMessage(MessageType::MERCHANT_PRODUCT_LIST_RESPONSE, response));
        return;
    }

    User* user = it->second;
    if (user->getUserType() != UserType::MERCHANT) {
        std::string response = "ERROR|只有商家才能查看商品";
        sendMessage(clientSocket, NetworkMessage(MessageType::MERCHANT_PRODUCT_LIST_RESPONSE, response));
        return;
    }

    std::string merchantName = user->getUsername();

    // 解析数据: page|pageSize
    std::istringstream iss(data);
    std::string pageStr, pageSizeStr;

    int page = 1;
    int pageSize = 10;

    if (std::getline(iss, pageStr, '|') && std::getline(iss, pageSizeStr)) {
        page = std::stoi(pageStr);
        pageSize = std::stoi(pageSizeStr);
    }

    std::vector<ProductInfo> products = productManager.getMerchantProductsByPage(merchantName, page, pageSize);
    int totalCount = productManager.getMerchantProductCount(merchantName);
    int totalPages = (totalCount + pageSize - 1) / pageSize;

    // 构建响应数据: totalPages|totalCount|currentPage|product1|product2|...
    // 每个商品格式: productId;name;originalPrice;currentPrice;stock;productType;discount
    std::ostringstream response;
    response << totalPages << "|" << totalCount << "|" << page;

    for (const auto& product : products) {
        response << "|" << product.productId << ";"
            << product.name << ";"
            << product.originalPrice << ";"
            << product.currentPrice << ";"
            << product.stock << ";"
            << product.productType << ";"
            << product.discount;
    }

    sendMessage(clientSocket, NetworkMessage(MessageType::MERCHANT_PRODUCT_LIST_RESPONSE, response.str()));
}

void Server::handleMerchantSetDiscountRequest(SOCKET clientSocket, const std::string& data) {
    // 检查用户是否已登录且为商家
    std::lock_guard<std::mutex> lock(clientsMutex);
    auto it = loggedInUsers.find(clientSocket);
    if (it == loggedInUsers.end()) {
        std::string response = "ERROR|请先登录";
        sendMessage(clientSocket, NetworkMessage(MessageType::MERCHANT_SET_DISCOUNT_RESPONSE, response));
        return;
    }

    User* user = it->second;
    if (user->getUserType() != UserType::MERCHANT) {
        std::string response = "ERROR|只有商家才能设置折扣";
        sendMessage(clientSocket, NetworkMessage(MessageType::MERCHANT_SET_DISCOUNT_RESPONSE, response));
        return;
    }

    // 解析数据: type|discount 或 productId|discount
    std::istringstream iss(data);
    std::string param1, discountStr;

    if (std::getline(iss, param1, '|') && std::getline(iss, discountStr)) {
        try {
            double discount = std::stod(discountStr);

            // 判断是按类型设置还是按商品ID设置
            if (param1 == "食品" || param1 == "书籍" || param1 == "衣服") {
                // 按类型设置折扣，但只对该商家的商品生效
                // 这里需要扩展功能，目前先设置所有该类型商品
                int count = productManager.setDiscountByType(param1, discount);

                std::string response = "SUCCESS|成功为 " + std::to_string(count) + " 个" + param1 + "商品设置折扣";
                sendMessage(clientSocket, NetworkMessage(MessageType::MERCHANT_SET_DISCOUNT_RESPONSE, response));
            }
            else {
                // 按商品ID设置折扣
                int productId = std::stoi(param1);

                // 验证商品是否属于该商家
                Product* product = productManager.getProductById(productId);
                if (!product || product->getMerchantName() != user->getUsername()) {
                    std::string response = "ERROR|商品不存在或不属于您";
                    sendMessage(clientSocket, NetworkMessage(MessageType::MERCHANT_SET_DISCOUNT_RESPONSE, response));
                    return;
                }

                if (productManager.modifyProduct(productId, -1, -1, discount)) {
                    std::string response = "SUCCESS|商品折扣设置成功";
                    sendMessage(clientSocket, NetworkMessage(MessageType::MERCHANT_SET_DISCOUNT_RESPONSE, response));
                }
                else {
                    std::string response = "ERROR|设置折扣失败";
                    sendMessage(clientSocket, NetworkMessage(MessageType::MERCHANT_SET_DISCOUNT_RESPONSE, response));
                }
            }
        }
        catch (const std::exception& e) {
            std::string response = "ERROR|数据格式错误: " + std::string(e.what());
            sendMessage(clientSocket, NetworkMessage(MessageType::MERCHANT_SET_DISCOUNT_RESPONSE, response));
        }
    }
    else {
        std::string response = "ERROR|折扣设置数据格式错误";
        sendMessage(clientSocket, NetworkMessage(MessageType::MERCHANT_SET_DISCOUNT_RESPONSE, response));
    }
}

void Server::handleCartAddItemRequest(SOCKET clientSocket, const std::string& data) {
    std::cout << "处理添加到购物车请求: " << data << std::endl;

    // 检查用户是否已登录且为消费者
    std::lock_guard<std::mutex> lock(clientsMutex);
    auto it = loggedInUsers.find(clientSocket);
    if (it == loggedInUsers.end()) {
        std::string response = "ERROR|请先登录";
        std::cout << "[DEBUG] 用户未登录，发送错误响应" << std::endl;
        sendMessage(clientSocket, NetworkMessage(MessageType::CART_ADD_ITEM_RESPONSE, response));
        return;
    }

    User* user = it->second;
    if (user->getUserType() != UserType::CONSUMER) {
        std::string response = "ERROR|只有消费者才能使用购物车";
        std::cout << "[DEBUG] 用户不是消费者，发送错误响应" << std::endl;
        sendMessage(clientSocket, NetworkMessage(MessageType::CART_ADD_ITEM_RESPONSE, response));
        return;
    }

    std::string username = user->getUsername();
    std::cout << "[DEBUG] 处理用户 " << username << " 的购物车请求" << std::endl;

    // 解析数据: productId|quantity
    std::istringstream iss(data);
    std::string productIdStr, quantityStr;

    if (std::getline(iss, productIdStr, '|') && std::getline(iss, quantityStr)) {
        try {
            int productId = std::stoi(productIdStr);
            int quantity = std::stoi(quantityStr);

            std::cout << "[DEBUG] 解析商品ID: " << productId << ", 数量: " << quantity << std::endl;

            if (quantity <= 0) {
                std::string response = "ERROR|商品数量必须大于0";
                std::cout << "[DEBUG] 数量无效，发送错误响应" << std::endl;
                sendMessage(clientSocket, NetworkMessage(MessageType::CART_ADD_ITEM_RESPONSE, response));
                return;
            }

            // 获取商品信息
            Product* product = productManager.getProductById(productId);
            if (!product) {
                std::string response = "ERROR|商品不存在";
                std::cout << "[DEBUG] 商品不存在，发送错误响应" << std::endl;
                sendMessage(clientSocket, NetworkMessage(MessageType::CART_ADD_ITEM_RESPONSE, response));
                return;
            }

            std::cout << "[DEBUG] 找到商品: " << product->getName() << ", 库存: " << product->getStock() << std::endl;

            // 检查库存
            if (!product->isAvailable(quantity)) {
                std::string response = "ERROR|库存不足，当前库存：" + std::to_string(product->getStock());
                std::cout << "[DEBUG] 库存不足，发送错误响应" << std::endl;
                sendMessage(clientSocket, NetworkMessage(MessageType::CART_ADD_ITEM_RESPONSE, response));
                return;
            }

            // 创建购物车项目
            CartItem item;
            item.productId = productId;
            item.productName = product->getName();
            item.productType = product->getProductType();
            item.originalPrice = product->getOriginalPrice();
            item.currentPrice = product->getPrice();
            item.quantity = quantity;
            item.merchantName = product->getMerchantName();
            item.discount = product->getDiscount();

            std::cout << "[DEBUG] 创建购物车项目完成，准备添加到购物车" << std::endl;

            // 添加到购物车
            if (cartManager.addItemToCart(username, item)) {
                std::string response = "SUCCESS|商品已成功添加到购物车";
                std::cout << "[DEBUG] 添加到购物车成功，发送成功响应" << std::endl;
                bool sendResult = sendMessage(clientSocket, NetworkMessage(MessageType::CART_ADD_ITEM_RESPONSE, response));
                std::cout << "[DEBUG] 发送响应结果: " << (sendResult ? "成功" : "失败") << std::endl;

                std::cout << "用户 [" << username << "] 添加商品到购物车: " << product->getName()
                    << " x" << quantity << std::endl;
            }
            else {
                std::string response = "ERROR|添加到购物车失败";
                std::cout << "[DEBUG] 添加到购物车失败，发送错误响应" << std::endl;
                sendMessage(clientSocket, NetworkMessage(MessageType::CART_ADD_ITEM_RESPONSE, response));
            }

        }
        catch (const std::exception& e) {
            std::string response = "ERROR|数据格式错误: " + std::string(e.what());
            std::cout << "[DEBUG] 数据解析异常: " << e.what() << ", 发送错误响应" << std::endl;
            sendMessage(clientSocket, NetworkMessage(MessageType::CART_ADD_ITEM_RESPONSE, response));
        }
    }
    else {
        std::string response = "ERROR|请求数据格式错误";
        std::cout << "[DEBUG] 请求数据格式错误，发送错误响应" << std::endl;
        sendMessage(clientSocket, NetworkMessage(MessageType::CART_ADD_ITEM_RESPONSE, response));
    }

    std::cout << "[DEBUG] handleCartAddItemRequest 处理完成" << std::endl;
}

void Server::handleCartViewRequest(SOCKET clientSocket, const std::string& data) {
    // 检查用户是否已登录且为消费者
    std::lock_guard<std::mutex> lock(clientsMutex);
    auto it = loggedInUsers.find(clientSocket);
    if (it == loggedInUsers.end()) {
        std::string response = "ERROR|请先登录";
        sendMessage(clientSocket, NetworkMessage(MessageType::CART_VIEW_RESPONSE, response));
        return;
    }

    User* user = it->second;
    if (user->getUserType() != UserType::CONSUMER) {
        std::string response = "ERROR|只有消费者才能查看购物车";
        sendMessage(clientSocket, NetworkMessage(MessageType::CART_VIEW_RESPONSE, response));
        return;
    }

    std::string username = user->getUsername();

    // 获取用户购物车
    std::vector<CartItem> cartItems = cartManager.getUserCartItems(username);
    double totalPrice = cartManager.getUserCartTotalPrice(username);
    int totalCount = cartManager.getUserCartItemCount(username);

    // 构建响应数据: SUCCESS|totalCount|totalPrice|item1|item2|...
    // 每个商品格式: productId;name;type;originalPrice;currentPrice;quantity;merchant;discount
    std::ostringstream response;
    response << "SUCCESS|" << totalCount << "|" << std::fixed << std::setprecision(2) << totalPrice;

    for (const auto& item : cartItems) {
        response << "|" << item.productId << ";"
            << item.productName << ";"
            << item.productType << ";"
            << item.originalPrice << ";"
            << item.currentPrice << ";"
            << item.quantity << ";"
            << item.merchantName << ";"
            << item.discount;
    }

    sendMessage(clientSocket, NetworkMessage(MessageType::CART_VIEW_RESPONSE, response.str()));
}

void Server::handleCartUpdateItemRequest(SOCKET clientSocket, const std::string& data) {
    // 检查用户是否已登录且为消费者
    std::lock_guard<std::mutex> lock(clientsMutex);
    auto it = loggedInUsers.find(clientSocket);
    if (it == loggedInUsers.end()) {
        std::string response = "ERROR|请先登录";
        sendMessage(clientSocket, NetworkMessage(MessageType::CART_UPDATE_ITEM_RESPONSE, response));
        return;
    }

    User* user = it->second;
    if (user->getUserType() != UserType::CONSUMER) {
        std::string response = "ERROR|只有消费者才能修改购物车";
        sendMessage(clientSocket, NetworkMessage(MessageType::CART_UPDATE_ITEM_RESPONSE, response));
        return;
    }

    std::string username = user->getUsername();

    // 解析数据: productId|quantity
    std::istringstream iss(data);
    std::string productIdStr, quantityStr;

    if (std::getline(iss, productIdStr, '|') && std::getline(iss, quantityStr)) {
        try {
            int productId = std::stoi(productIdStr);
            int quantity = std::stoi(quantityStr);

            if (quantity < 0) {
                std::string response = "ERROR|商品数量不能为负数";
                sendMessage(clientSocket, NetworkMessage(MessageType::CART_UPDATE_ITEM_RESPONSE, response));
                return;
            }

            // 如果数量为0，相当于删除商品
            if (quantity == 0) {
                if (cartManager.removeItemFromCart(username, productId)) {
                    std::string response = "SUCCESS|商品已从购物车中移除";
                    sendMessage(clientSocket, NetworkMessage(MessageType::CART_UPDATE_ITEM_RESPONSE, response));
                }
                else {
                    std::string response = "ERROR|移除商品失败";
                    sendMessage(clientSocket, NetworkMessage(MessageType::CART_UPDATE_ITEM_RESPONSE, response));
                }
                return;
            }

            // 检查商品是否存在以及库存
            Product* product = productManager.getProductById(productId);
            if (!product) {
                std::string response = "ERROR|商品不存在";
                sendMessage(clientSocket, NetworkMessage(MessageType::CART_UPDATE_ITEM_RESPONSE, response));
                return;
            }

            if (!product->isAvailable(quantity)) {
                std::string response = "ERROR|库存不足，当前库存：" + std::to_string(product->getStock());
                sendMessage(clientSocket, NetworkMessage(MessageType::CART_UPDATE_ITEM_RESPONSE, response));
                return;
            }

            // 更新购物车商品数量
            if (cartManager.updateCartItem(username, productId, quantity)) {
                std::string response = "SUCCESS|商品数量已更新";
                sendMessage(clientSocket, NetworkMessage(MessageType::CART_UPDATE_ITEM_RESPONSE, response));
            }
            else {
                std::string response = "ERROR|更新商品数量失败";
                sendMessage(clientSocket, NetworkMessage(MessageType::CART_UPDATE_ITEM_RESPONSE, response));
            }

        }
        catch (const std::exception& e) {
            std::string response = "ERROR|数据格式错误: " + std::string(e.what());
            sendMessage(clientSocket, NetworkMessage(MessageType::CART_UPDATE_ITEM_RESPONSE, response));
        }
    }
    else {
        std::string response = "ERROR|请求数据格式错误";
        sendMessage(clientSocket, NetworkMessage(MessageType::CART_UPDATE_ITEM_RESPONSE, response));
    }
}

void Server::handleCartRemoveItemRequest(SOCKET clientSocket, const std::string& data) {
    // 检查用户是否已登录且为消费者
    std::lock_guard<std::mutex> lock(clientsMutex);
    auto it = loggedInUsers.find(clientSocket);
    if (it == loggedInUsers.end()) {
        std::string response = "ERROR|请先登录";
        sendMessage(clientSocket, NetworkMessage(MessageType::CART_REMOVE_ITEM_RESPONSE, response));
        return;
    }

    User* user = it->second;
    if (user->getUserType() != UserType::CONSUMER) {
        std::string response = "ERROR|只有消费者才能修改购物车";
        sendMessage(clientSocket, NetworkMessage(MessageType::CART_REMOVE_ITEM_RESPONSE, response));
        return;
    }

    std::string username = user->getUsername();

    // 解析数据: productId
    try {
        int productId = std::stoi(data);

        // 从购物车移除商品
        if (cartManager.removeItemFromCart(username, productId)) {
            std::string response = "SUCCESS|商品已从购物车中移除";
            sendMessage(clientSocket, NetworkMessage(MessageType::CART_REMOVE_ITEM_RESPONSE, response));
        }
        else {
            std::string response = "ERROR|商品不在购物车中或移除失败";
            sendMessage(clientSocket, NetworkMessage(MessageType::CART_REMOVE_ITEM_RESPONSE, response));
        }

    }
    catch (const std::exception& e) {
        std::string response = "ERROR|商品ID格式错误: " + std::string(e.what());
        sendMessage(clientSocket, NetworkMessage(MessageType::CART_REMOVE_ITEM_RESPONSE, response));
    }
}

void Server::handleCartClearRequest(SOCKET clientSocket, const std::string& data) {
    // 检查用户是否已登录且为消费者
    std::lock_guard<std::mutex> lock(clientsMutex);
    auto it = loggedInUsers.find(clientSocket);
    if (it == loggedInUsers.end()) {
        std::string response = "ERROR|请先登录";
        sendMessage(clientSocket, NetworkMessage(MessageType::CART_CLEAR_RESPONSE, response));
        return;
    }

    User* user = it->second;
    if (user->getUserType() != UserType::CONSUMER) {
        std::string response = "ERROR|只有消费者才能清空购物车";
        sendMessage(clientSocket, NetworkMessage(MessageType::CART_CLEAR_RESPONSE, response));
        return;
    }

    std::string username = user->getUsername();

    // 清空用户购物车
    if (cartManager.clearUserCart(username)) {
        std::string response = "SUCCESS|购物车已清空";
        sendMessage(clientSocket, NetworkMessage(MessageType::CART_CLEAR_RESPONSE, response));
    }
    else {
        std::string response = "ERROR|清空购物车失败";
        sendMessage(clientSocket, NetworkMessage(MessageType::CART_CLEAR_RESPONSE, response));
    }
}

void Server::handleOrderCheckoutRequest(SOCKET clientSocket, const std::string& data) {
    std::cout << "处理订单结算请求" << std::endl;

    // 检查用户是否已登录且为消费者
    std::lock_guard<std::mutex> lock(clientsMutex);
    auto it = loggedInUsers.find(clientSocket);
    if (it == loggedInUsers.end()) {
        std::string response = "ERROR|请先登录";
        sendMessage(clientSocket, NetworkMessage(MessageType::ORDER_CHECKOUT_RESPONSE, response));
        return;
    }

    User* user = it->second;
    if (user->getUserType() != UserType::CONSUMER) {
        std::string response = "ERROR|只有消费者才能下订单";
        sendMessage(clientSocket, NetworkMessage(MessageType::ORDER_CHECKOUT_RESPONSE, response));
        return;
    }

    std::string username = user->getUsername();

    // 获取用户购物车
    std::vector<CartItem> cartItems = cartManager.getUserCartItems(username);
    if (cartItems.empty()) {
        std::string response = "ERROR|购物车为空，无法结算";
        sendMessage(clientSocket, NetworkMessage(MessageType::ORDER_CHECKOUT_RESPONSE, response));
        return;
    }

    // 计算总价
    double totalPrice = cartManager.getUserCartTotalPrice(username);

    // 检查用户余额
    if (user->getBalance() < totalPrice) {
        std::string response = "ERROR|余额不足，当前余额：" + std::to_string(user->getBalance()) +
            "，需要：" + std::to_string(totalPrice);
        sendMessage(clientSocket, NetworkMessage(MessageType::ORDER_CHECKOUT_RESPONSE, response));
        return;
    }

    // 记录各商家应得金额和订单商品（只定义一次）
    std::map<std::string, double> merchantEarnings;
    std::map<std::string, std::vector<std::string>> merchantOrderItems;

    // 检查库存并预扣除，同时计算商家收入
    std::vector<std::pair<int, int>> stockOperations; // 记录库存操作，以便回滚

    for (const auto& item : cartItems) {
        Product* product = productManager.getProductById(item.productId);
        if (!product) {
            // 回滚之前的库存操作
            for (auto& op : stockOperations) {
                Product* rollbackProduct = productManager.getProductById(op.first);
                if (rollbackProduct) {
                    rollbackProduct->setStock(rollbackProduct->getStock() + op.second);
                }
            }

            std::string response = "ERROR|商品[ID:" + std::to_string(item.productId) + "]不存在";
            sendMessage(clientSocket, NetworkMessage(MessageType::ORDER_CHECKOUT_RESPONSE, response));
            return;
        }

        if (!product->isAvailable(item.quantity)) {
            // 回滚之前的库存操作
            for (auto& op : stockOperations) {
                Product* rollbackProduct = productManager.getProductById(op.first);
                if (rollbackProduct) {
                    rollbackProduct->setStock(rollbackProduct->getStock() + op.second);
                }
            }

            std::string response = "ERROR|商品[" + product->getName() + "]库存不足，当前库存：" +
                std::to_string(product->getStock());
            sendMessage(clientSocket, NetworkMessage(MessageType::ORDER_CHECKOUT_RESPONSE, response));
            return;
        }

        // 扣除库存
        int newStock = product->getStock() - item.quantity;
        product->setStock(newStock);
        stockOperations.push_back({ item.productId, item.quantity });

        // 计算商家收入
        std::string merchantName = product->getMerchantName();
        double itemTotal = item.currentPrice * item.quantity;
        merchantEarnings[merchantName] += itemTotal;

        std::cout << "商品: " << product->getName()
            << ", 商家: " << merchantName
            << ", 收入: " << itemTotal << std::endl;

        // 记录商家的订单商品
        std::string itemInfo = product->getName() + "|数量:" + std::to_string(item.quantity) +
            "|单价:" + std::to_string(item.currentPrice) +
            "|小计:" + std::to_string(itemTotal) +
            "|客户:" + username;
        merchantOrderItems[merchantName].push_back(itemInfo);
    }

    // 扣除消费者余额
    double newBalance = user->getBalance() - totalPrice;
    user->setBalance(newBalance);

    // 生成订单时间和订单ID
    time_t now = time(0);
    char timeStr[100];
    strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", localtime(&now));

    // 简单的订单ID生成（时间戳）
    int orderId = static_cast<int>(now);

    // 为消费者写入订单记录：orders_消费者用户名.txt
    std::string customerOrderFile = "orders_" + username + ".txt";
    std::ofstream customerFile(customerOrderFile, std::ios::app);

    if (customerFile.is_open()) {
        customerFile << "订单ID:" << orderId << std::endl;
        customerFile << "时间:" << timeStr << std::endl;
        customerFile << "总金额:" << totalPrice << std::endl;
        customerFile << "状态:已完成" << std::endl;
        customerFile << "商品明细:" << std::endl;

        // 写入商品详情
        for (const auto& item : cartItems) {
            Product* product = productManager.getProductById(item.productId);
            if (product) {
                customerFile << "  " << product->getName()
                    << "|数量:" << item.quantity
                    << "|单价:" << item.currentPrice
                    << "|小计:" << (item.currentPrice * item.quantity)
                    << "|商家:" << product->getMerchantName() << std::endl;
            }
        }

        customerFile << "---订单结束---" << std::endl;
        customerFile.close();
        std::cout << "已为消费者 [" << username << "] 记录订单到文件 " << customerOrderFile << std::endl;
    }

    // 为每个商家写入订单记录：orders_商家用户名.txt
    for (const auto& earning : merchantEarnings) {
        std::string merchantName = earning.first;
        double amount = earning.second;

        std::cout << "为商家 [" << merchantName << "] 写入收入记录: " << amount << " 元" << std::endl;

        std::string merchantOrderFile = "orders_" + merchantName + ".txt";
        std::ofstream merchantFile(merchantOrderFile, std::ios::app);

        if (merchantFile.is_open()) {
            merchantFile << "订单ID:" << orderId << std::endl;
            merchantFile << "时间:" << timeStr << std::endl;
            merchantFile << "客户:" << username << std::endl;
            merchantFile << "收入:" << amount << std::endl;
            merchantFile << "状态:已完成" << std::endl;
            merchantFile << "商品明细:" << std::endl;

            // 写入该商家相关的商品
            for (const auto& itemInfo : merchantOrderItems[merchantName]) {
                merchantFile << "  " << itemInfo << std::endl;
            }

            merchantFile << "---订单结束---" << std::endl;
            merchantFile.close();

            std::cout << "已为商家 [" << merchantName << "] 记录订单到文件 " << merchantOrderFile
                << "，收入: " << amount << " 元" << std::endl;
        }
        else {
            std::cerr << "无法打开商家订单文件: " << merchantOrderFile << std::endl;
        }
    }

    std::cout << "订单创建成功:" << std::endl;
    std::cout << "订单ID: " << orderId << std::endl;
    std::cout << "用户: " << username << std::endl;
    std::cout << "时间: " << timeStr << std::endl;
    std::cout << "总价: " << totalPrice << std::endl;
    std::cout << "商品数量: " << cartItems.size() << std::endl;

    // 清空用户购物车
    cartManager.clearUserCart(username);

    // 保存数据
    userManager.saveUsers();
    productManager.saveProducts();

    std::string response = "SUCCESS|订单创建成功，订单ID：" + std::to_string(orderId) +
        "，订单金额：" + std::to_string(totalPrice) +
        "，余额：" + std::to_string(newBalance);
    sendMessage(clientSocket, NetworkMessage(MessageType::ORDER_CHECKOUT_RESPONSE, response));

    std::cout << "用户 [" << username << "] 完成订单结算，订单ID: " << orderId << std::endl;
}

void Server::handleOrderListRequest(SOCKET clientSocket, const std::string& data) {
    std::cout << "处理订单列表请求" << std::endl;

    // 检查用户是否已登录
    std::lock_guard<std::mutex> lock(clientsMutex);
    auto it = loggedInUsers.find(clientSocket);
    if (it == loggedInUsers.end()) {
        std::string response = "ERROR|请先登录";
        sendMessage(clientSocket, NetworkMessage(MessageType::ORDER_LIST_RESPONSE, response));
        return;
    }

    User* user = it->second;
    std::string username = user->getUsername();
    std::string userTypeStr = (user->getUserType() == UserType::CONSUMER) ? "消费者" : "商家";

    std::cout << userTypeStr << " [" << username << "] 查看订单列表" << std::endl;

    // 读取用户订单文件：orders_用户名.txt
    std::string userOrderFile = "orders_" + username + ".txt";
    std::ifstream file(userOrderFile);

    std::vector<std::string> orderList;

    if (file.is_open()) {
        std::string line;
        std::string orderIdLine, timeLine, amountLine, statusLine, customerLine;
        bool inOrder = false;

        while (std::getline(file, line)) {
            if (line.find("订单ID:") == 0) {
                // 如果之前有订单，先保存
                if (inOrder && !orderIdLine.empty() && !timeLine.empty() && !statusLine.empty()) {
                    std::string orderInfo = orderIdLine + "|" + timeLine + "|";

                    if (user->getUserType() == UserType::CONSUMER) {
                        // 消费者格式：订单ID|时间|总金额|状态
                        orderInfo += amountLine + "|" + statusLine;
                    }
                    else {
                        // 商家格式：订单ID|时间|客户|收入|状态
                        orderInfo += customerLine + "|" + amountLine + "|" + statusLine;
                    }

                    orderList.push_back(orderInfo);
                    std::cout << "添加订单到列表: " << orderInfo << std::endl;
                }

                // 重置并开始新订单
                orderIdLine = line;
                timeLine = "";
                amountLine = "";
                statusLine = "";
                customerLine = "";
                inOrder = true;

            }
            else if (line.find("时间:") == 0) {
                timeLine = line;
            }
            else if (line.find("总金额:") == 0 && user->getUserType() == UserType::CONSUMER) {
                amountLine = line;
            }
            else if (line.find("收入:") == 0 && user->getUserType() == UserType::MERCHANT) {
                amountLine = line;
            }
            else if (line.find("客户:") == 0 && user->getUserType() == UserType::MERCHANT) {
                customerLine = line;
            }
            else if (line.find("状态:") == 0) {
                statusLine = line;
            }
            else if (line == "---订单结束---") {
                // 订单结束，保存当前订单
                if (inOrder && !orderIdLine.empty() && !timeLine.empty() && !statusLine.empty()) {
                    std::string orderInfo = orderIdLine + "|" + timeLine + "|";

                    if (user->getUserType() == UserType::CONSUMER) {
                        // 消费者格式：订单ID|时间|总金额|状态
                        orderInfo += amountLine + "|" + statusLine;
                    }
                    else {
                        // 商家格式：订单ID|时间|客户|收入|状态
                        orderInfo += customerLine + "|" + amountLine + "|" + statusLine;
                    }

                    orderList.push_back(orderInfo);
                    std::cout << "保存订单: " << orderInfo << std::endl;
                }

                // 重置状态，准备下一个订单
                inOrder = false;
                orderIdLine = "";
                timeLine = "";
                amountLine = "";
                statusLine = "";
                customerLine = "";
            }
        }

        // 处理最后一个订单（如果文件没有以---订单结束---结尾）
        if (inOrder && !orderIdLine.empty() && !timeLine.empty() && !statusLine.empty()) {
            std::string orderInfo;
            if (user->getUserType() == UserType::CONSUMER) {
                orderInfo = orderIdLine + "|" + timeLine + "|" + amountLine + "|" + statusLine;
            }
            else {
                orderInfo = orderIdLine + "|" + timeLine + "|" + customerLine + "|" + amountLine + "|" + statusLine;
            }
            orderList.push_back(orderInfo);
            std::cout << "添加最后订单到列表: " << orderInfo << std::endl;
        }

        file.close();
    }
    else {
        std::cout << "用户订单文件不存在: " << userOrderFile << std::endl;
    }

    // 构造响应
    std::string response;
    if (orderList.empty()) {
        response = "SUCCESS|暂无订单记录";
    }
    else {
        response = "SUCCESS|" + std::to_string(orderList.size());
        for (const auto& order : orderList) {
            response += "|" + order;
        }
    }

    std::cout << "最终响应数据: " << response << std::endl;

    sendMessage(clientSocket, NetworkMessage(MessageType::ORDER_LIST_RESPONSE, response));
    std::cout << "返回 " << orderList.size() << " 条订单记录给 " << userTypeStr << " [" << username << "]" << std::endl;
}

bool Server::sendMessage(SOCKET clientSocket, const NetworkMessage& message) {
    // 添加调试信息
    std::cout << "[DEBUG] 服务器发送消息类型: " << static_cast<int>(message.type)
        << ", 数据: " << message.data << std::endl;

    std::vector<char> buffer = message.serialize();

    int totalSent = 0;
    int bufferSize = static_cast<int>(buffer.size());

    while (totalSent < bufferSize) {
        int sent = send(clientSocket, buffer.data() + totalSent, bufferSize - totalSent, 0);
        if (sent == SOCKET_ERROR) {
            std::cerr << "发送消息失败: " << WSAGetLastError() << std::endl;
            return false;
        }
        totalSent += sent;
    }

    std::cout << "[DEBUG] 服务器成功发送 " << totalSent << " 字节" << std::endl;
    return true;
}

void Server::broadcastMessage(const NetworkMessage& message) {
    std::lock_guard<std::mutex> lock(clientsMutex);
    for (SOCKET clientSocket : clientSockets) {
        sendMessage(clientSocket, message);
    }
}

void Server::stop() {
    running = false;

    if (serverSocket != INVALID_SOCKET) {
        closesocket(serverSocket);
        serverSocket = INVALID_SOCKET;
    }

    // 关闭所有客户端连接
    std::lock_guard<std::mutex> lock(clientsMutex);
    for (SOCKET clientSocket : clientSockets) {
        closesocket(clientSocket);
    }
    clientSockets.clear();
    clientInfo.clear();
    loggedInUsers.clear();
}