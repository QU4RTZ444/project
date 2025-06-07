#include "client.h"
#include "ui_manager.h"
#include "utils.h"
#include <iostream>
#include <sstream>
#include <windows.h>

Client::Client() : clientSocket(INVALID_SOCKET), connected(false), userBalance(0.0),
currentPage(1), totalPages(0), totalCount(0), waitingForResponse(false),
cartTotalPrice(0.0), cartTotalCount(0) {
    // 初始化Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup失败" << std::endl;
    }
}

Client::~Client() {
    disconnect();
    WSACleanup();
}

void Client::disconnect() {
    if (connected) {
        connected = false;

        if (clientSocket != INVALID_SOCKET) {
            closesocket(clientSocket);
            clientSocket = INVALID_SOCKET;
        }

        // 等待接收线程结束
        if (receiveThread.joinable()) {
            receiveThread.join();
        }

        std::cout << "[DEBUG] 客户端断开连接" << std::endl;
    }
}

bool Client::connectToServer(const std::string& serverIP, int port) {
    // 创建socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "创建socket失败: " << WSAGetLastError() << std::endl;
        return false;
    }

    // 设置服务器地址
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    inet_pton(AF_INET, serverIP.c_str(), &serverAddr.sin_addr);

    // 连接到服务器
    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "连接服务器失败: " << WSAGetLastError() << std::endl;
        closesocket(clientSocket);
        clientSocket = INVALID_SOCKET;
        return false;
    }

    connected = true;

    // 启动接收消息的线程
    receiveThread = std::thread(&Client::receiveMessages, this);

    std::cout << "[DEBUG] 连接成功，接收线程已启动" << std::endl;

    return true;
}

void Client::receiveMessages() {
    char buffer[4096];

    while (connected) {
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);

        if (bytesReceived > 0) {
            try {
                std::vector<char> messageBuffer(buffer, buffer + bytesReceived);
                NetworkMessage message = NetworkMessage::deserialize(messageBuffer);

                // 添加调试信息
                std::cout << "[DEBUG] 客户端收到消息类型: " << static_cast<int>(message.type)
                    << ", 数据: " << message.data << std::endl;

                handleMessage(message);
            }
            catch (const std::exception& e) {
                std::cerr << "处理接收到的消息时出错: " << e.what() << std::endl;
                // 添加调试信息
                std::cout << "[DEBUG] 收到的原始数据 (" << bytesReceived << " 字节): ";
                for (int i = 0; i < bytesReceived && i < 100; ++i) {
                    std::cout << static_cast<int>(buffer[i]) << " ";
                }
                std::cout << std::endl;
            }
        }
        else if (bytesReceived == 0) {
            std::cout << "服务器断开连接" << std::endl;
            connected = false;
            break;
        }
        else {
            int error = WSAGetLastError();
            if (error != WSAEWOULDBLOCK) {
                std::cerr << "接收消息失败: " << error << std::endl;
                connected = false;
                break;
            }
        }

        Sleep(10);
    }

    std::cout << "[DEBUG] 接收线程退出" << std::endl;
}

void Client::handleMessage(const NetworkMessage& message) {
    try {
        switch (message.type) {
        case MessageType::CONNECT_RESPONSE:
            Utils::showSuccess("服务器连接响应: " + message.data);
            break;

        case MessageType::REGISTER_RESPONSE:
        {
            std::istringstream iss(message.data);
            std::string status, msg;
            if (std::getline(iss, status, '|') && std::getline(iss, msg)) {
                if (status == "SUCCESS") {
                    Utils::showSuccess(msg);
                }
                else {
                    Utils::showError(msg);
                }
            }
            else {
                Utils::showInfo("注册响应: " + message.data);
            }
            Utils::pauseScreen();
        }
        break;

        case MessageType::LOGIN_RESPONSE:
        {
            std::istringstream iss(message.data);
            std::string status, msg, type, balance;
            if (std::getline(iss, status, '|')) {
                if (status == "SUCCESS" &&
                    std::getline(iss, msg, '|') &&
                    std::getline(iss, type, '|') &&
                    std::getline(iss, balance)) {

                    userType = type;
                    userBalance = std::stod(balance);
                    Utils::showSuccess(msg);
                    Utils::showInfo("用户类型: " + userType);
                    Utils::showInfo("账户余额: " + Utils::formatMoney(userBalance));
                }
                else if (status == "ERROR" && std::getline(iss, msg)) {
                    Utils::showError(msg);
                    currentUser.clear(); // 登录失败，清除用户名
                }
            }
            else {
                Utils::showInfo("登录响应: " + message.data);
            }
            Utils::pauseScreen();
        }
        break;

        case MessageType::LOGOUT_RESPONSE:
        {
            std::istringstream iss(message.data);
            std::string status, msg;
            if (std::getline(iss, status, '|') && std::getline(iss, msg)) {
                if (status == "SUCCESS") {
                    Utils::showSuccess(msg);
                    currentUser.clear();
                    userType.clear();
                    userBalance = 0.0;
                }
                else {
                    Utils::showError(msg);
                }
            }
            else {
                Utils::showInfo("登出响应: " + message.data);
            }
            Utils::pauseScreen();
        }
        break;

        case MessageType::CHANGE_PASSWORD_RESPONSE:
        {
            std::istringstream iss(message.data);
            std::string status, msg;
            if (std::getline(iss, status, '|') && std::getline(iss, msg)) {
                if (status == "SUCCESS") {
                    Utils::showSuccess(msg);
                }
                else {
                    Utils::showError(msg);
                }
            }
            else {
                Utils::showInfo("密码修改响应: " + message.data);
            }
            Utils::pauseScreen();
        }
        break;

        case MessageType::SUCCESS_RESPONSE:
            Utils::showSuccess("服务器回复: " + message.data);
            break;

        case MessageType::ERROR_RESPONSE:
            Utils::showError("服务器错误: " + message.data);
            break;

        case MessageType::PRODUCT_LIST_RESPONSE:
        {
            std::istringstream iss(message.data);
            std::string totalPagesStr, totalCountStr, currentPageStr;

            if (std::getline(iss, totalPagesStr, '|') &&
                std::getline(iss, totalCountStr, '|') &&
                std::getline(iss, currentPageStr, '|')) {

                totalPages = std::stoi(totalPagesStr);
                totalCount = std::stoull(totalCountStr);
                currentPage = std::stoi(currentPageStr);

                currentProducts.clear();

                std::string productData;
                while (std::getline(iss, productData, '|')) {
                    std::istringstream productIss(productData);
                    std::string idStr, name, originalPriceStr, currentPriceStr, stockStr, merchant, productType, discountStr;

                    if (std::getline(productIss, idStr, ';') &&
                        std::getline(productIss, name, ';') &&
                        std::getline(productIss, originalPriceStr, ';') &&
                        std::getline(productIss, currentPriceStr, ';') &&
                        std::getline(productIss, stockStr, ';') &&
                        std::getline(productIss, merchant, ';') &&
                        std::getline(productIss, productType, ';') &&
                        std::getline(productIss, discountStr, ';')) {

                        ProductInfo product;
                        product.id = std::stoi(idStr);
                        product.name = name;
                        product.originalPrice = std::stod(originalPriceStr);
                        product.price = std::stod(currentPriceStr);
                        product.stock = std::stoi(stockStr);
                        product.merchant = merchant;
                        product.productType = productType;
                        product.discount = std::stod(discountStr);

                        currentProducts.push_back(product);
                    }
                }
            }
            waitingForResponse = false;
        }
        break;

        case MessageType::PRODUCT_SEARCH_RESPONSE:
        {
            std::istringstream iss(message.data);
            std::string countStr;

            if (std::getline(iss, countStr, '|')) {
                size_t count = std::stoull(countStr);
                currentProducts.clear();

                std::string productData;
                while (std::getline(iss, productData, '|')) {
                    std::istringstream productIss(productData);
                    std::string idStr, name, originalPriceStr, currentPriceStr, stockStr, merchant, productType, discountStr;

                    if (std::getline(productIss, idStr, ';') &&
                        std::getline(productIss, name, ';') &&
                        std::getline(productIss, originalPriceStr, ';') &&
                        std::getline(productIss, currentPriceStr, ';') &&
                        std::getline(productIss, stockStr, ';') &&
                        std::getline(productIss, merchant, ';') &&
                        std::getline(productIss, productType, ';') &&
                        std::getline(productIss, discountStr, ';')) {

                        ProductInfo product;
                        product.id = std::stoi(idStr);
                        product.name = name;
                        product.originalPrice = std::stod(originalPriceStr);
                        product.price = std::stod(currentPriceStr);
                        product.stock = std::stoi(stockStr);
                        product.merchant = merchant;
                        product.productType = productType;
                        product.discount = std::stod(discountStr);

                        currentProducts.push_back(product);
                    }
                }

                Utils::showInfo("找到 " + std::to_string(count) + " 个商品");
            }
            waitingForResponse = false;
        }
        break;

        case MessageType::PRODUCT_DETAIL_RESPONSE:
        {
            std::istringstream iss(message.data);
            std::string status;

            if (std::getline(iss, status, '|')) {
                if (status == "SUCCESS") {
                    std::string idStr, name, description, originalPriceStr, currentPriceStr, stockStr, merchant, productType, discountStr;

                    if (std::getline(iss, idStr, '|') &&
                        std::getline(iss, name, '|') &&
                        std::getline(iss, description, '|') &&
                        std::getline(iss, originalPriceStr, '|') &&
                        std::getline(iss, currentPriceStr, '|') &&
                        std::getline(iss, stockStr, '|') &&
                        std::getline(iss, merchant, '|') &&
                        std::getline(iss, productType, '|') &&
                        std::getline(iss, discountStr, '|')) {

                        UIManager::showProductDetail();
                        std::cout << "商品ID: " << idStr << std::endl;
                        std::cout << "商品名称: " << name << std::endl;
                        std::cout << "商品类型: " << productType << std::endl;

                        double discount = std::stod(discountStr);
                        double originalPrice = std::stod(originalPriceStr);
                        double currentPrice = std::stod(currentPriceStr);
                        int stock = std::stoi(stockStr);

                        if (discount < 1.0) {
                            std::cout << "原价: " << Utils::formatMoney(originalPrice) << std::endl;
                            std::cout << "折扣: " << static_cast<int>(discount * 100) << "折" << std::endl;
                            std::cout << "现价: " << Utils::formatMoney(currentPrice) << std::endl;
                        }
                        else {
                            std::cout << "价格: " << Utils::formatMoney(currentPrice) << std::endl;
                        }

                        std::cout << "库存: " << stock << std::endl;
                        std::cout << "商家: " << merchant << std::endl;

                        bool addedToCart = false;

                        // 如果用户已登录且是消费者，显示加入购物车选项
                        if (!currentUser.empty() && userType == "消费者") {
                            std::cout << std::endl;
                            std::cout << "是否加入购物车？ (y/n): ";
                            std::string choice;
                            std::cin >> choice;
                            if (choice == "y" || choice == "Y") {
                                // 询问购买数量
                                std::cout << "请输入购买数量 (1-" << stock << "): ";
                                std::string quantityStr;
                                std::cin >> quantityStr;

                                try {
                                    int quantity = std::stoi(quantityStr);
                                    if (quantity > 0 && quantity <= stock) {
                                        // 发送添加到购物车请求
                                        handleAddToCart(std::stoi(idStr), quantity);
                                        addedToCart = true;
                                    }
                                    else {
                                        Utils::showError("购买数量无效！");
                                        Utils::pauseScreen();
                                    }
                                }
                                catch (const std::exception& e) {
                                    Utils::showError("购买数量格式错误！");
                                    Utils::pauseScreen();
                                }
                            }
                        }
                        else if (currentUser.empty()) {
                            Utils::showInfo("登录后可加入购物车");
                        }

                        // 如果没有进行购物车操作，需要暂停等待用户按键
                        if (!addedToCart) {
                            Utils::pauseScreen();
                        }
                    }
                }
                else {
                    std::string errorMsg;
                    std::getline(iss, errorMsg);
                    Utils::showError(errorMsg);
                    Utils::pauseScreen();
                }
            }
            waitingForResponse = false;
            // 移除这里的自动暂停，因为上面已经根据情况处理了
            // Utils::pauseScreen();
        }
        break;

        case MessageType::CART_ADD_ITEM_RESPONSE:
        {
            std::istringstream iss(message.data);
            std::string status, msg;
            if (std::getline(iss, status, '|') && std::getline(iss, msg)) {
                if (status == "SUCCESS") {
                    Utils::showSuccess(msg);
                }
                else {
                    Utils::showError(msg);
                }
            }
            else {
                Utils::showInfo("添加到购物车响应: " + message.data);
            }
            waitingForResponse = false;
            // 不在这里暂停，让调用方决定
            // Utils::pauseScreen();
        }
        break;

        case MessageType::CART_VIEW_RESPONSE:
        {
            std::istringstream iss(message.data);
            std::string status;

            if (std::getline(iss, status, '|')) {
                if (status == "SUCCESS") {
                    std::string totalCountStr, totalPriceStr;

                    if (std::getline(iss, totalCountStr, '|') &&
                        std::getline(iss, totalPriceStr, '|')) {

                        cartTotalCount = std::stoi(totalCountStr);
                        cartTotalPrice = std::stod(totalPriceStr);
                        currentCartItems.clear();

                        std::string itemData;
                        while (std::getline(iss, itemData, '|')) {
                            std::istringstream itemIss(itemData);
                            std::string idStr, name, type, origPriceStr, currPriceStr, qtyStr, merchant, discountStr;

                            if (std::getline(itemIss, idStr, ';') &&
                                std::getline(itemIss, name, ';') &&
                                std::getline(itemIss, type, ';') &&
                                std::getline(itemIss, origPriceStr, ';') &&
                                std::getline(itemIss, currPriceStr, ';') &&
                                std::getline(itemIss, qtyStr, ';') &&
                                std::getline(itemIss, merchant, ';') &&
                                std::getline(itemIss, discountStr)) {

                                CartItemInfo item;
                                item.id = std::stoi(idStr);
                                item.name = name;
                                item.type = type;
                                item.originalPrice = std::stod(origPriceStr);
                                item.currentPrice = std::stod(currPriceStr);
                                item.quantity = std::stoi(qtyStr);
                                item.merchant = merchant;
                                item.discount = std::stod(discountStr);

                                currentCartItems.push_back(item);
                            }
                        }

                        // 显示购物车内容
                        UIManager::showCartDetail();

                        if (currentCartItems.empty()) {
                            std::cout << "购物车为空" << std::endl;
                        }
                        else {
                            std::cout << "购物车商品 (共 " << cartTotalCount << " 件商品)：" << std::endl;
                            std::cout << "============================================================" << std::endl;

                            for (size_t i = 0; i < currentCartItems.size(); ++i) {
                                const auto& item = currentCartItems[i];
                                std::cout << (i + 1) << ". " << item.name;

                                if (item.discount < 1.0) {
                                    std::cout << "\n   价格: " << Utils::formatMoney(item.currentPrice)
                                        << " (原价:" << Utils::formatMoney(item.originalPrice)
                                        << ", " << static_cast<int>(item.discount * 100) << "折)";
                                }
                                else {
                                    std::cout << "\n   价格: " << Utils::formatMoney(item.currentPrice);
                                }

                                std::cout << "\n   数量: " << item.quantity
                                    << "   小计: " << Utils::formatMoney(item.getTotalPrice())
                                    << "\n   商家: " << item.merchant
                                    << "   类型: " << item.type << std::endl;

                                if (i < currentCartItems.size() - 1) {
                                    std::cout << "------------------------------------------------------------" << std::endl;
                                }
                            }

                            std::cout << "============================================================" << std::endl;
                            std::cout << "总计: " << Utils::formatMoney(cartTotalPrice) << " 元" << std::endl;
                        }
                    }
                }
                else {
                    std::string errorMsg;
                    std::getline(iss, errorMsg);
                    Utils::showError(errorMsg);
                }
            }
            waitingForResponse = false;
            // 不在这里暂停
        }
        break;

        case MessageType::CART_UPDATE_ITEM_RESPONSE:
        case MessageType::CART_REMOVE_ITEM_RESPONSE:
        case MessageType::CART_CLEAR_RESPONSE:
        {
            std::istringstream iss(message.data);
            std::string status, msg;
            if (std::getline(iss, status, '|') && std::getline(iss, msg)) {
                if (status == "SUCCESS") {
                    Utils::showSuccess(msg);
                }
                else {
                    Utils::showError(msg);
                }
            }
            else {
                Utils::showInfo("操作响应: " + message.data);
            }
            waitingForResponse = false;
            // 不在这里暂停
        }
        break;

        case MessageType::ORDER_CHECKOUT_RESPONSE:
        {
            std::istringstream iss(message.data);
            std::string status, msg;
            if (std::getline(iss, status, '|') && std::getline(iss, msg)) {
                // 静默处理响应，不干扰用户体验
                std::cout << "[后台] 订单处理确认: " << (status == "SUCCESS" ? "成功" : "失败") << std::endl;
                if (status == "SUCCESS") {
                    // 可以解析更新后的用户余额等信息
                    std::cout << "[后台] " << msg << std::endl;
                }
            }
        }
        break;

        case MessageType::ORDER_LIST_RESPONSE:
        {
            std::istringstream iss(message.data);
            std::string status;

            if (std::getline(iss, status, '|')) {
                if (status == "SUCCESS") {
                    std::string countStr;
                    if (std::getline(iss, countStr, '|')) {
                        UIManager::showOrderList();

                        if (countStr == "暂无订单记录") {
                            std::cout << "暂无订单记录" << std::endl;
                        }
                        else {
                            try {
                                int orderCount = std::stoi(countStr);

                                if (userType == "消费者") {
                                    std::cout << "您的购买订单记录：" << std::endl;
                                }
                                else {
                                    std::cout << "您的销售订单记录：" << std::endl;
                                }

                                std::cout << "共找到 " << orderCount << " 条订单记录：" << std::endl;
                                std::cout << "========================================================" << std::endl;

                                // 读取剩余的所有内容
                                std::string remainingData;
                                std::string part;
                                while (std::getline(iss, part, '|')) {
                                    if (!remainingData.empty()) {
                                        remainingData += " | ";
                                    }
                                    remainingData += part;
                                }

                                // 使用"已完成"作为分隔符分割订单
                                std::string delimiter = " | 状态:已完成";
                                size_t pos = 0;
                                int orderIndex = 1;

                                while ((pos = remainingData.find(delimiter)) != std::string::npos) {
                                    std::string orderInfo = remainingData.substr(0, pos + delimiter.length());

                                    if (userType == "消费者") {
                                        // 消费者订单格式：订单ID | 时间 | 总金额 | 状态
                                        std::cout << orderIndex << ". " << orderInfo << std::endl;
                                    }
                                    else {
                                        // 商家订单格式：订单ID | 时间 | 客户 | 收入 | 状态
                                        // 为商家订单添加标识
                                        std::cout << orderIndex << ". " << orderInfo << std::endl;
                                    }

                                    if (orderIndex < orderCount) {
                                        std::cout << "--------------------------------------------------------" << std::endl;
                                    }

                                    remainingData.erase(0, pos + delimiter.length());
                                    // 跳过可能的分隔符
                                    if (remainingData.find(" | ") == 0) {
                                        remainingData.erase(0, 3);
                                    }
                                    orderIndex++;
                                }

                                // 处理最后一个订单（如果有的话）
                                if (!remainingData.empty() && orderIndex <= orderCount) {
                                    // 确保最后一个订单以"已完成"结尾
                                    if (remainingData.find("状态:已完成") != std::string::npos) {
                                        if (userType == "消费者") {
                                            std::cout << orderIndex << ". " << remainingData << std::endl;
                                        }
                                        else {
                                            std::cout << orderIndex << ". " << remainingData << std::endl;
                                        }
                                    }
                                }

                                std::cout << "========================================================" << std::endl;

                                // 显示订单统计信息
                                if (userType == "商家") {
                                    std::cout << std::endl;
                                    std::cout << "提示：以上显示的是您商品的销售记录" << std::endl;
                                    std::cout << "每笔订单的收入已自动计入您的账户余额" << std::endl;
                                }
                                else {
                                    std::cout << std::endl;
                                    std::cout << "提示：以上显示的是您的购买记录" << std::endl;
                                }

                            }
                            catch (const std::exception& e) {
                                Utils::showError("解析订单数量时出错");

                                // 调试：显示原始数据
                                std::cout << "调试信息 - 原始响应数据: " << message.data << std::endl;
                            }
                        }
                    }
                }
                else {
                    std::string errorMsg;
                    std::getline(iss, errorMsg);
                    Utils::showError(errorMsg);
                }
            }
            waitingForResponse = false;
        }
        break;

        default:
            Utils::showInfo("收到未知消息类型: " + std::to_string(static_cast<int>(message.type)));
            break;
        }
    }
    catch (const std::exception& e) {
        Utils::showError("处理消息内容时出错: " + std::string(e.what()));
        waitingForResponse = false;
    }
}

void Client::handleRegister() {
    UIManager::showRegisterForm();

    std::string username = Utils::getInput("请输入用户名: ");
    if (username.empty()) {
        Utils::showError("用户名不能为空！");
        Utils::pauseScreen();
        return;
    }

    std::string password = Utils::getInput("请输入密码: ");
    if (password.empty()) {
        Utils::showError("密码不能为空！");
        Utils::pauseScreen();
        return;
    }

    Utils::showSeparator("选择用户类型");
    std::cout << "1. 消费者" << std::endl;
    std::cout << "2. 商家" << std::endl;
    std::string userTypeChoice = UIManager::getMenuChoice(2);

    std::string data = username + "|" + password + "|" + userTypeChoice;
    NetworkMessage message(MessageType::REGISTER_REQUEST, data);

    if (sendMessage(message)) {
        UIManager::showLoading("注册请求已发送，等待服务器响应");
        Sleep(500);
    }
    else {
        Utils::showError("发送注册请求失败");
        Utils::pauseScreen();
    }
}

void Client::handleLogin() {
    UIManager::showLoginForm();

    std::string username = Utils::getInput("请输入用户名: ");
    if (username.empty()) {
        Utils::showError("用户名不能为空！");
        Utils::pauseScreen();
        return;
    }

    std::string password = Utils::getInput("请输入密码: ");
    if (password.empty()) {
        Utils::showError("密码不能为空！");
        Utils::pauseScreen();
        return;
    }

    std::string data = username + "|" + password;
    NetworkMessage message(MessageType::LOGIN_REQUEST, data);

    if (sendMessage(message)) {
        currentUser = username;
        UIManager::showLoading("登录请求已发送，等待服务器响应");
        Sleep(500);
    }
    else {
        Utils::showError("发送登录请求失败");
        Utils::pauseScreen();
    }
}

void Client::handleLogout() {
    NetworkMessage message(MessageType::LOGOUT_REQUEST, "");
    if (sendMessage(message)) {
        UIManager::showLoading("登出请求已发送，等待服务器响应");
        Sleep(500);
    }
    else {
        Utils::showError("发送登出请求失败");
        Utils::pauseScreen();
    }
}

bool Client::sendMessage(const NetworkMessage& message) {
    if (!connected) {
        return false;
    }

    try {
        std::vector<char> buffer = message.serialize();

        // 添加调试信息
        std::cout << "[DEBUG] 发送消息类型: " << static_cast<int>(message.type)
            << ", 数据: " << message.data << std::endl;

        int totalSent = 0;
        int bufferSize = static_cast<int>(buffer.size());

        while (totalSent < bufferSize) {
            int sent = send(clientSocket, buffer.data() + totalSent, bufferSize - totalSent, 0);
            if (sent == SOCKET_ERROR) {
                int error = WSAGetLastError();
                std::cerr << "发送消息失败: " << error << std::endl;
                return false;
            }
            totalSent += sent;
        }

        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "序列化消息时出错: " << e.what() << std::endl;
        return false;
    }
}

void Client::handleAccountManagement() {
    while (true) {
        UIManager::showAccountMenu();
        std::string choice = UIManager::getMenuChoice(5);

        if (choice == "1") {
            handleChangePassword();
        }
        else if (choice == "2") {
            Utils::clearScreen();
            UIManager::showSystemTitle();
            Utils::showSeparator("账户信息");
            std::cout << "用户名: " << currentUser << std::endl;
            std::cout << "用户类型: " << userType << std::endl;
            std::cout << "账户余额: " << Utils::formatMoney(userBalance) << std::endl;
            Utils::pauseScreen();
        }
        else if (choice == "3") {
            Utils::showInfo("账户充值功能正在开发中...");
            Utils::pauseScreen();
        }
        else if (choice == "4") {
            Utils::showInfo("账户提现功能正在开发中...");
            Utils::pauseScreen();
        }
        else if (choice == "5") {
            break; // 返回主菜单
        }
    }
}

void Client::handleChangePassword() {
    UIManager::showChangePasswordForm();

    std::string oldPassword, newPassword, confirmPassword;

    // 清理输入流
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::cout << "请输入当前密码: ";
    std::getline(std::cin, oldPassword);

    if (oldPassword.empty()) {
        Utils::showError("当前密码不能为空！");
        Utils::pauseScreen();
        return;
    }

    std::cout << "请输入新密码: ";
    std::getline(std::cin, newPassword);

    if (newPassword.empty()) {
        Utils::showError("新密码不能为空！");
        Utils::pauseScreen();
        return;
    }

    if (newPassword.length() < 6) {
        Utils::showError("新密码长度不能少于6位！");
        Utils::pauseScreen();
        return;
    }

    std::cout << "请确认新密码: ";
    std::getline(std::cin, confirmPassword);

    if (newPassword != confirmPassword) {
        Utils::showError("两次输入的新密码不一致！");
        Utils::pauseScreen();
        return;
    }

    std::string data = oldPassword + "|" + newPassword;
    NetworkMessage message(MessageType::CHANGE_PASSWORD_REQUEST, data);

    if (sendMessage(message)) {
        UIManager::showLoading("密码修改请求已发送，等待服务器响应");
        Sleep(500);
    }
    else {
        Utils::showError("发送密码修改请求失败");
        Utils::pauseScreen();
    }
}

void Client::handleProductBrowse() {
    while (true) {
        UIManager::showProductBrowseMenu();
        std::string choice = UIManager::getMenuChoice(3);

        if (choice == "1") {
            handleProductList();
        }
        else if (choice == "2") {
            handleProductSearch();
        }
        else if (choice == "3") {
            break; // 返回主菜单
        }
    }
}

void Client::handleProductList() {
    currentPage = 1;

    while (true) {
        // 请求商品列表
        std::string data = std::to_string(currentPage) + "|5"; // 每页5个商品
        NetworkMessage message(MessageType::PRODUCT_LIST_REQUEST, data);

        waitingForResponse = true;
        if (sendMessage(message)) {
            UIManager::showLoading("正在加载商品列表");

            // 等待响应
            while (waitingForResponse && connected) {
                Sleep(100);
            }

            if (!connected) break;

            // 显示商品列表
            UIManager::showProductList();

            if (currentProducts.empty()) {
                Utils::showInfo("暂无商品");
                Utils::pauseScreen();
                break;
            }

            std::cout << "第 " << currentPage << " 页，共 " << totalPages << " 页 (总共 " << totalCount << " 个商品)\n" << std::endl;

            for (size_t i = 0; i < currentProducts.size(); ++i) {
                const auto& product = currentProducts[i];
                std::cout << i + 1 << ". [" << product.id << "] " << product.name;

                if (product.discount < 1.0) {
                    std::cout << " - " << Utils::formatMoney(product.price)
                        << " (原价:" << Utils::formatMoney(product.originalPrice)
                        << ", " << static_cast<int>(product.discount * 100) << "折)";
                }
                else {
                    std::cout << " - " << Utils::formatMoney(product.price);
                }

                std::cout << " (库存:" << product.stock << ") [" << product.merchant << "]"
                    << " {" << product.productType << "}" << std::endl;
            }

            std::cout << std::endl;
            std::cout << "操作选项:" << std::endl;
            std::cout << "1-" << currentProducts.size() << ". 查看商品详情" << std::endl;

            if (currentPage > 1) {
                std::cout << "p. 上一页" << std::endl;
            }
            if (currentPage < totalPages) {
                std::cout << "n. 下一页" << std::endl;
            }
            std::cout << "b. 返回上级菜单" << std::endl;

            std::cout << "\n请选择操作: ";
            std::string choice;
            std::cin >> choice;

            if (choice == "b" || choice == "B") {
                break;
            }
            else if (choice == "p" || choice == "P") {
                if (currentPage > 1) {
                    currentPage--;
                }
            }
            else if (choice == "n" || choice == "N") {
                if (currentPage < totalPages) {
                    currentPage++;
                }
            }
            else {
                try {
                    int index = std::stoi(choice);
                    if (index >= 1 && index <= static_cast<int>(currentProducts.size())) {
                        handleProductDetail(currentProducts[index - 1].id);
                    }
                    else {
                        Utils::showError("无效选择");
                        Utils::pauseScreen();
                    }
                }
                catch (const std::exception&) {
                    Utils::showError("无效选择");
                    Utils::pauseScreen();
                }
            }
        }
        else {
            Utils::showError("发送请求失败");
            Utils::pauseScreen();
            break;
        }
    }
}

void Client::handleProductSearch() {
    UIManager::showProductSearchForm();

    std::string keyword;

    // 清理输入流
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::cout << "请输入搜索关键词: ";
    std::getline(std::cin, keyword);

    if (keyword.empty()) {
        Utils::showError("搜索关键词不能为空！");
        Utils::pauseScreen();
        return;
    }

    NetworkMessage message(MessageType::PRODUCT_SEARCH_REQUEST, keyword);

    waitingForResponse = true;
    if (sendMessage(message)) {
        UIManager::showLoading("正在搜索商品");

        // 等待响应
        while (waitingForResponse && connected) {
            Sleep(100);
        }

        if (!connected) return;

        // 显示搜索结果
        Utils::clearScreen();
        UIManager::showSystemTitle();
        Utils::showSeparator("搜索结果");

        if (currentProducts.empty()) {
            Utils::showInfo("未找到相关商品");
            Utils::pauseScreen();
            return;
        }

        for (size_t i = 0; i < currentProducts.size(); ++i) {
            const auto& product = currentProducts[i];
            std::cout << i + 1 << ". [" << product.id << "] " << product.name
                << " - " << Utils::formatMoney(product.price)
                << " (库存:" << product.stock << ") [" << product.merchant << "]"
                << " {" << product.productType << "}" << std::endl;
        }

        std::cout << std::endl;
        std::cout << "1-" << currentProducts.size() << ". 查看商品详情" << std::endl;
        std::cout << "b. 返回" << std::endl;

        std::cout << "\n请选择操作: ";
        std::string choice;
        std::cin >> choice;

        if (choice != "b" && choice != "B") {
            try {
                int index = std::stoi(choice);
                if (index >= 1 && index <= static_cast<int>(currentProducts.size())) {
                    handleProductDetail(currentProducts[index - 1].id);
                }
                else {
                    Utils::showError("无效选择");
                    Utils::pauseScreen();
                }
            }
            catch (const std::exception&) {
                Utils::showError("无效选择");
                Utils::pauseScreen();
            }
        }
    }
    else {
        Utils::showError("发送搜索请求失败");
        Utils::pauseScreen();
    }
}

void Client::handleProductDetail(int productId) {
    NetworkMessage message(MessageType::PRODUCT_DETAIL_REQUEST, std::to_string(productId));

    waitingForResponse = true;
    if (sendMessage(message)) {
        UIManager::showLoading("正在加载商品详情");

        // 等待响应
        while (waitingForResponse && connected) {
            Sleep(100);
        }

        // 商品详情显示完成后，如果没有进行购物车操作，也要暂停等待用户按键
        if (connected) {
            // 注意：如果在handleMessage中已经进行了购物车操作并暂停，这里就不需要再暂停
            // 但为了保险起见，我们可以在这里检查一下是否需要暂停
        }
    }
    else {
        Utils::showError("发送请求失败");
        Utils::pauseScreen();
    }
}

void Client::handleMerchantProductManagement() {
    while (true) {
        UIManager::showMerchantProductMenu();
        std::string choice = UIManager::getMenuChoice(5);

        if (choice == "1") {
            handleMerchantAddProduct();
        }
        else if (choice == "2") {
            handleMerchantProductList();
        }
        else if (choice == "3") {
            handleMerchantModifyProduct();
        }
        else if (choice == "4") {
            handleMerchantSetDiscount();
        }
        else if (choice == "5") {
            break; // 返回主菜单
        }
    }
}

void Client::handleMerchantAddProduct() {
    UIManager::showAddProductForm();

    std::string typeChoice, name, priceStr, stockStr, discountStr;

    // 清理输入流
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::cout << "请选择商品类型：" << std::endl;
    std::cout << "1. 食品" << std::endl;
    std::cout << "2. 书籍" << std::endl;
    std::cout << "3. 衣服" << std::endl;
    std::cout << "请输入选择 (1-3): ";
    std::getline(std::cin, typeChoice);

    std::string type;
    if (typeChoice == "1") {
        type = "食品";
    }
    else if (typeChoice == "2") {
        type = "书籍";
    }
    else if (typeChoice == "3") {
        type = "衣服";
    }
    else {
        Utils::showError("无效的选择！请选择1、2或3");
        Utils::pauseScreen();
        return;
    }

    std::cout << "请输入商品名称: ";
    std::getline(std::cin, name);

    if (name.empty()) {
        Utils::showError("商品名称不能为空！");
        Utils::pauseScreen();
        return;
    }

    std::cout << "请输入商品价格: ";
    std::getline(std::cin, priceStr);

    std::cout << "请输入商品库存: ";
    std::getline(std::cin, stockStr);

    std::cout << "请输入商品折扣 (1.0表示无折扣, 0.8表示8折): ";
    std::getline(std::cin, discountStr);

    try {
        double price = std::stod(priceStr);
        int stock = std::stoi(stockStr);
        double discount = std::stod(discountStr);

        if (price <= 0) {
            Utils::showError("商品价格必须大于0！");
            Utils::pauseScreen();
            return;
        }

        if (stock < 0) {
            Utils::showError("商品库存不能为负数！");
            Utils::pauseScreen();
            return;
        }

        if (discount <= 0 || discount > 1.0) {
            Utils::showError("折扣必须在0.0到1.0之间！");
            Utils::pauseScreen();
            return;
        }

        std::string data = type + "|" + name + "|" + priceStr + "|" + stockStr + "|" + discountStr;
        NetworkMessage message(MessageType::MERCHANT_ADD_PRODUCT_REQUEST, data);

        waitingForResponse = true;
        if (sendMessage(message)) {
            UIManager::showLoading("正在添加商品");

            // 等待响应
            while (waitingForResponse && connected) {
                Sleep(100);
            }
        }
        else {
            Utils::showError("发送添加商品请求失败");
            Utils::pauseScreen();
        }
    }
    catch (const std::exception& e) {
        Utils::showError("输入数据格式错误: " + std::string(e.what()));
        Utils::pauseScreen();
    }
}

void Client::handleMerchantProductList() {
    currentPage = 1;

    while (true) {
        // 请求商家商品列表
        std::string data = std::to_string(currentPage) + "|10"; // 每页10个商品
        NetworkMessage message(MessageType::MERCHANT_PRODUCT_LIST_REQUEST, data);

        waitingForResponse = true;
        if (sendMessage(message)) {
            UIManager::showLoading("正在加载商品列表");

            // 等待响应
            while (waitingForResponse && connected) {
                Sleep(100);
            }

            if (!connected) break;

            // 显示商品列表
            UIManager::showMerchantProductList();

            if (currentProducts.empty()) {
                Utils::showInfo("您还没有添加任何商品");
                Utils::pauseScreen();
                break;
            }

            std::cout << "第 " << currentPage << " 页，共 " << totalPages << " 页 (总共 " << totalCount << " 个商品)\n" << std::endl;

            for (size_t i = 0; i < currentProducts.size(); ++i) {
                const auto& product = currentProducts[i];
                std::cout << i + 1 << ". [" << product.id << "] " << product.name
                    << " - " << Utils::formatMoney(product.price);

                if (product.discount < 1.0) {
                    std::cout << " (" << static_cast<int>(product.discount * 100) << "折)";
                }

                std::cout << " (库存:" << product.stock << ") {" << product.productType << "}" << std::endl;
            }

            std::cout << std::endl;
            std::cout << "操作选项:" << std::endl;

            if (currentPage > 1) {
                std::cout << "p. 上一页" << std::endl;
            }
            if (currentPage < totalPages) {
                std::cout << "n. 下一页" << std::endl;
            }
            std::cout << "b. 返回上级菜单" << std::endl;

            std::cout << "\n请选择操作: ";
            std::string choice;
            std::cin >> choice;

            if (choice == "b" || choice == "B") {
                break;
            }
            else if (choice == "p" || choice == "P") {
                if (currentPage > 1) {
                    currentPage--;
                }
            }
            else if (choice == "n" || choice == "N") {
                if (currentPage < totalPages) {
                    currentPage++;
                }
            }
        }
        else {
            Utils::showError("发送请求失败");
            Utils::pauseScreen();
            break;
        }
    }
}

void Client::handleMerchantModifyProduct() {
    // 先获取商家的商品列表
    std::string data = "1|100"; // 获取第1页，每页100个商品（显示所有商品）
    NetworkMessage message(MessageType::MERCHANT_PRODUCT_LIST_REQUEST, data);

    waitingForResponse = true;
    if (!sendMessage(message)) {
        Utils::showError("发送商品列表请求失败");
        Utils::pauseScreen();
        return;
    }

    UIManager::showLoading("正在加载您的商品列表");

    // 等待响应
    while (waitingForResponse && connected) {
        Sleep(100);
    }

    if (!connected) return;

    UIManager::showModifyProductForm();

    if (currentProducts.empty()) {
        Utils::showError("您还没有添加任何商品！");
        Utils::pauseScreen();
        return;
    }

    // 显示商家的商品列表
    std::cout << "您的商品列表：" << std::endl;
    std::cout << "------------------------------------------------------------" << std::endl;

    for (size_t i = 0; i < currentProducts.size(); ++i) {
        const auto& product = currentProducts[i];
        std::cout << (i + 1) << ". [ID:" << product.id << "] " << product.name;

        if (product.discount < 1.0) {
            std::cout << " - " << Utils::formatMoney(product.price)
                << " (原价:" << Utils::formatMoney(product.originalPrice)
                << ", " << static_cast<int>(product.discount * 100) << "折)";
        }
        else {
            std::cout << " - " << Utils::formatMoney(product.price);
        }

        std::cout << " (库存:" << product.stock << ") {" << product.productType << "}" << std::endl;
    }

    std::cout << "------------------------------------------------------------" << std::endl;
    std::cout << std::endl;

    // 让用户选择要修改的商品
    std::cout << "请选择要修改的商品 (输入序号 1-" << currentProducts.size() << "，0取消): ";
    std::string choiceStr;
    std::cin >> choiceStr;

    if (choiceStr == "0") {
        return;
    }

    try {
        int choice = std::stoi(choiceStr);
        if (choice < 1 || choice > static_cast<int>(currentProducts.size())) {
            Utils::showError("无效的选择！");
            Utils::pauseScreen();
            return;
        }

        // 选中的商品
        const auto& selectedProduct = currentProducts[choice - 1];

        // 显示当前商品信息
        std::cout << std::endl;
        std::cout << "选中商品信息：" << std::endl;
        std::cout << "商品ID: " << selectedProduct.id << std::endl;
        std::cout << "商品名称: " << selectedProduct.name << std::endl;
        std::cout << "商品类型: " << selectedProduct.productType << std::endl;
        std::cout << "当前原价: " << Utils::formatMoney(selectedProduct.originalPrice) << std::endl;
        std::cout << "当前折扣: " << static_cast<int>(selectedProduct.discount * 100) << "折" << std::endl;
        std::cout << "当前现价: " << Utils::formatMoney(selectedProduct.price) << std::endl;
        std::cout << "当前库存: " << selectedProduct.stock << std::endl;
        std::cout << std::endl;

        // 获取修改信息
        std::string priceStr, stockStr, discountStr;

        // 清理输入流
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        std::cout << "请输入新的原价 (当前: " << Utils::formatMoney(selectedProduct.originalPrice)
            << ", 不修改请按回车): ";
        std::getline(std::cin, priceStr);

        std::cout << "请输入新的库存 (当前: " << selectedProduct.stock
            << ", 不修改请按回车): ";
        std::getline(std::cin, stockStr);

        std::cout << "请输入新的折扣 (当前: " << selectedProduct.discount
            << ", 不修改请按回车): ";
        std::getline(std::cin, discountStr);

        // 处理空输入（不修改的字段）
        if (priceStr.empty()) priceStr = "-1";
        if (stockStr.empty()) stockStr = "-1";
        if (discountStr.empty()) discountStr = "-1";

        // 验证输入
        try {
            if (priceStr != "-1") {
                double price = std::stod(priceStr);
                if (price <= 0) {
                    Utils::showError("商品价格必须大于0！");
                    Utils::pauseScreen();
                    return;
                }
            }

            if (stockStr != "-1") {
                int stock = std::stoi(stockStr);
                if (stock < 0) {
                    Utils::showError("商品库存不能为负数！");
                    Utils::pauseScreen();
                    return;
                }
            }

            if (discountStr != "-1") {
                double discount = std::stod(discountStr);
                if (discount <= 0 || discount > 1.0) {
                    Utils::showError("折扣必须在0.0到1.0之间！");
                    Utils::pauseScreen();
                    return;
                }
            }
        }
        catch (const std::exception& e) {
            Utils::showError("输入数据格式错误: " + std::string(e.what()));
            Utils::pauseScreen();
            return;
        }

        // 发送修改请求
        std::string requestData = std::to_string(selectedProduct.id) + "|" + priceStr + "|" + stockStr + "|" + discountStr;
        NetworkMessage modifyMessage(MessageType::MERCHANT_MODIFY_PRODUCT_REQUEST, requestData);

        waitingForResponse = true;
        if (sendMessage(modifyMessage)) {
            UIManager::showLoading("正在修改商品");

            // 等待响应
            while (waitingForResponse && connected) {
                Sleep(100);
            }
        }
        else {
            Utils::showError("发送修改商品请求失败");
            Utils::pauseScreen();
        }

    }
    catch (const std::exception& e) {
        Utils::showError("输入格式错误: " + std::string(e.what()));
        Utils::pauseScreen();
    }
}

void Client::handleMerchantSetDiscount() {
    UIManager::showSetDiscountForm();

    std::cout << "1. 为指定商品设置折扣" << std::endl;
    std::cout << "2. 为指定类型商品设置折扣" << std::endl;
    std::cout << "请选择 (1-2): ";

    std::string choice;
    std::cin >> choice;

    std::string param, discountStr;

    if (choice == "1") {
        // 为指定商品设置折扣 - 先显示商品列表
        std::string data = "1|100"; // 获取第1页，每页100个商品
        NetworkMessage message(MessageType::MERCHANT_PRODUCT_LIST_REQUEST, data);

        waitingForResponse = true;
        if (!sendMessage(message)) {
            Utils::showError("发送商品列表请求失败");
            Utils::pauseScreen();
            return;
        }

        UIManager::showLoading("正在加载您的商品列表");

        // 等待响应
        while (waitingForResponse && connected) {
            Sleep(100);
        }

        if (!connected) return;

        if (currentProducts.empty()) {
            Utils::showError("您还没有添加任何商品！");
            Utils::pauseScreen();
            return;
        }

        // 清理输入流
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        // 显示商品列表
        std::cout << std::endl << "您的商品列表：" << std::endl;
        std::cout << "------------------------------------------------------------" << std::endl;

        for (size_t i = 0; i < currentProducts.size(); ++i) {
            const auto& product = currentProducts[i];
            std::cout << (i + 1) << ". [ID:" << product.id << "] " << product.name
                << " - " << Utils::formatMoney(product.price)
                << " (当前折扣: " << static_cast<int>(product.discount * 100) << "折)"
                << " {" << product.productType << "}" << std::endl;
        }

        std::cout << "------------------------------------------------------------" << std::endl;

        std::cout << "请选择要设置折扣的商品 (输入序号 1-" << currentProducts.size() << "，0取消): ";
        std::string choiceStr;
        std::getline(std::cin, choiceStr);

        if (choiceStr == "0") {
            return;
        }

        try {
            int productChoice = std::stoi(choiceStr);
            if (productChoice < 1 || productChoice > static_cast<int>(currentProducts.size())) {
                Utils::showError("无效的选择！");
                Utils::pauseScreen();
                return;
            }

            const auto& selectedProduct = currentProducts[productChoice - 1];
            param = std::to_string(selectedProduct.id);

            std::cout << "选中商品: " << selectedProduct.name
                << " (当前折扣: " << static_cast<int>(selectedProduct.discount * 100) << "折)" << std::endl;

        }
        catch (const std::exception& e) {
            Utils::showError("输入格式错误: " + std::string(e.what()));
            Utils::pauseScreen();
            return;
        }

    }
    else if (choice == "2") {
        // 清理输入流
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        std::cout << "请选择商品类型：" << std::endl;
        std::cout << "1. 食品" << std::endl;
        std::cout << "2. 书籍" << std::endl;
        std::cout << "3. 衣服" << std::endl;
        std::cout << "请输入选择 (1-3): ";

        std::string typeChoice;
        std::getline(std::cin, typeChoice);

        if (typeChoice == "1") {
            param = "食品";
        }
        else if (typeChoice == "2") {
            param = "书籍";
        }
        else if (typeChoice == "3") {
            param = "衣服";
        }
        else {
            Utils::showError("无效的选择！");
            Utils::pauseScreen();
            return;
        }
    }
    else {
        Utils::showError("无效选择！");
        Utils::pauseScreen();
        return;
    }

    std::cout << "请输入折扣 (例如：0.8表示8折): ";
    std::getline(std::cin, discountStr);

    try {
        double discount = std::stod(discountStr);

        if (discount <= 0 || discount > 1.0) {
            Utils::showError("折扣必须在0.0到1.0之间！");
            Utils::pauseScreen();
            return;
        }

        std::string data = param + "|" + discountStr;
        NetworkMessage message(MessageType::MERCHANT_SET_DISCOUNT_REQUEST, data);

        waitingForResponse = true;
        if (sendMessage(message)) {
            UIManager::showLoading("正在设置折扣");

            // 等待响应
            while (waitingForResponse && connected) {
                Sleep(100);
            }
        }
        else {
            Utils::showError("发送设置折扣请求失败");
            Utils::pauseScreen();
        }
    }
    catch (const std::exception& e) {
        Utils::showError("输入数据格式错误: " + std::string(e.what()));
        Utils::pauseScreen();
    }
}

void Client::run() {
    // 显示欢迎信息
    UIManager::showWelcomeMessage();

    while (connected) {
        try {
            if (currentUser.empty()) {
                // 未登录状态
                UIManager::showMainMenu();
                std::string choice = UIManager::getMenuChoice(4);

                if (choice == "1") {
                    handleRegister();
                }
                else if (choice == "2") {
                    handleLogin();
                }
                else if (choice == "3") {
                    handleProductBrowse();
                }
                else if (choice == "4") {
                    disconnect();
                    break;
                }
            }
            else {
                // 已登录状态 - 根据用户类型显示不同菜单
                if (userType == "消费者") {
                    UIManager::showConsumerMenu();
                    std::string choice;
                    std::cin >> choice;

                    if (choice == "1") {
                        handleProductList();
                    }
                    else if (choice == "2") {
                        handleProductSearch();
                    }
                    else if (choice == "3") {
                        handleCartManagement(); // 购物车管理
                    }
                    else if (choice == "4") {
                        handleOrderManagement(); // 订单管理
                    }
                    else if (choice == "5") {
                        handleChangePassword();
                    }
                    else if (choice == "6") {
                        handleLogout();
                    }
                    else {
                        Utils::showError("无效选择！");
                        Utils::pauseScreen();
                    }
                }
                else {
                    // 商家菜单
                    UIManager::showLoggedInMenu(currentUser, userType, userBalance);
                    std::string choice = UIManager::getMenuChoice(6);

                    if (choice == "1") {
                        handleProductList();
                    }
                    else if (choice == "2") {
                        handleProductSearch();
                    }
                    else if (choice == "3") {
                        handleMerchantProductManagement();
                    }
                    else if (choice == "4") {
                        handleOrderManagement(); // 订单管理（对商家和消费者都适用）
                    }
                    else if (choice == "5") {
                        handleAccountManagement();
                    }
                    else if (choice == "6") {
                        handleLogout();
                    }
                }
            }

            Sleep(100);
        }
        catch (const std::exception& e) {
            Utils::showError("客户端运行时出错: " + std::string(e.what()));
            Utils::pauseScreen();
            break;
        }
    }

    UIManager::showExitMessage();
    disconnect();
}

void Client::handleAddToCart(int productId, int quantity) {
    std::string data = std::to_string(productId) + "|" + std::to_string(quantity);
    NetworkMessage message(MessageType::CART_ADD_ITEM_REQUEST, data);

    std::cout << "正在添加商品到购物车..." << std::endl;

    waitingForResponse = true;
    if (sendMessage(message)) {
        // 等待响应，添加超时机制
        int timeout = 0;
        //while (waitingForResponse && connected && timeout < 50) { // 5秒超时
        //    Sleep(100);
        //    timeout++;
        //}

        //if (timeout >= 50) {
        //    std::cout << "添加到购物车超时，请重试" << std::endl;
        //    waitingForResponse = false;
        //}

        // 等待用户按键再返回
        Utils::pauseScreen();
    }
    else {
        Utils::showError("发送添加到购物车请求失败");
        Utils::pauseScreen();
    }
}

void Client::handleCartManagement() {
    while (connected && !currentUser.empty()) {
        UIManager::showCartMenu();

        std::string choice;
        std::cin >> choice;

        if (choice == "1") {
            handleViewCart();
        }
        else if (choice == "2") {
            handleUpdateCartItem();
        }
        else if (choice == "3") {
            handleRemoveCartItem();
        }
        else if (choice == "4") {
            handleClearCart();
        }
        else if (choice == "5") {
            handleCheckout(); // 添加结算功能
        }
        else if (choice == "6") {
            break; // 返回上级菜单
        }
        else {
            Utils::showError("无效选择！");
            Utils::pauseScreen();
        }
    }
}

void Client::handleCheckout() {
    // 先查看购物车，确保有商品
    NetworkMessage message(MessageType::CART_VIEW_REQUEST, "");

    waitingForResponse = true;
    if (!sendMessage(message)) {
        Utils::showError("获取购物车信息失败");
        Utils::pauseScreen();
        return;
    }

    UIManager::showLoading("正在加载购物车信息");

    // 等待响应
    while (waitingForResponse && connected) {
        Sleep(100);
    }

    if (!connected) return;

    if (currentCartItems.empty()) {
        Utils::showError("购物车为空，无法结算！");
        Utils::pauseScreen();
        return;
    }

    // 显示结算界面
    UIManager::showCheckoutConfirmation();

    // 显示购物车商品和总计
    std::cout << "订单商品列表：" << std::endl;
    for (size_t i = 0; i < currentCartItems.size(); ++i) {
        const auto& item = currentCartItems[i];
        std::cout << (i + 1) << ". " << item.name;

        if (item.discount < 1.0) {
            std::cout << "\n   价格: " << Utils::formatMoney(item.currentPrice)
                << " (原价:" << Utils::formatMoney(item.originalPrice)
                << ", " << static_cast<int>(item.discount * 100) << "折)";
        }
        else {
            std::cout << "\n   价格: " << Utils::formatMoney(item.currentPrice);
        }

        std::cout << "\n   数量: " << item.quantity
            << "   小计: " << Utils::formatMoney(item.getTotalPrice())
            << "\n   商家: " << item.merchant << std::endl;

        if (i < currentCartItems.size() - 1) {
            std::cout << "------------------------------------------------------------" << std::endl;
        }
    }

    std::cout << "============================================================" << std::endl;
    std::cout << "订单总计: " << Utils::formatMoney(cartTotalPrice) << " 元" << std::endl;
    std::cout << "当前余额: " << Utils::formatMoney(userBalance) << " 元" << std::endl;

    // 检查余额是否足够
    if (userBalance < cartTotalPrice) {
        std::cout << "余额不足! 还需要: " << Utils::formatMoney(cartTotalPrice - userBalance) << " 元" << std::endl;
        Utils::showError("余额不足，无法完成支付");
        Utils::pauseScreen();
        return;
    }

    std::cout << "支付后余额: " << Utils::formatMoney(userBalance - cartTotalPrice) << " 元" << std::endl;
    std::cout << std::endl;

    // 确认订单
    std::cout << "确认提交订单并支付吗？ (y/n): ";
    std::string confirm;
    std::cin >> confirm;

    if (confirm == "y" || confirm == "Y") {
        handleConfirmOrder();
    }
    else {
        std::cout << "订单已取消" << std::endl;
        Utils::pauseScreen();
    }
}

void Client::handleConfirmOrder() {
    // 发送结算请求
    NetworkMessage message(MessageType::ORDER_CHECKOUT_REQUEST, "");

    std::cout << "正在处理订单..." << std::endl;

    if (sendMessage(message)) {
        // 直接显示处理中，不等待响应（按照之前的处理方式）
        std::cout << "订单提交成功！" << std::endl;
        std::cout << "系统正在处理您的订单，请稍后查看订单状态" << std::endl;

        // 清空本地购物车缓存（因为服务端会清空购物车）
        currentCartItems.clear();
        cartTotalPrice = 0.0;
        cartTotalCount = 0;

        // 更新本地余额（估算，实际以服务端为准）
        userBalance -= cartTotalPrice;

        Utils::showSuccess("订单提交完成");
    }
    else {
        Utils::showError("订单提交失败，请重试");
    }

    Utils::pauseScreen();
}

void Client::handleViewCart() {
    NetworkMessage message(MessageType::CART_VIEW_REQUEST, "");

    waitingForResponse = true;
    if (sendMessage(message)) {
        UIManager::showLoading("正在加载购物车");

        // 等待响应
        while (waitingForResponse && connected) {
            Sleep(100);
        }

        // 响应处理完成后，等待用户按键
        if (connected) {
            std::cout << std::endl;
            Utils::pauseScreen(); // 等待用户按任意键继续
        }
    }
    else {
        Utils::showError("发送购物车查看请求失败");
        Utils::pauseScreen();
    }
}

void Client::handleUpdateCartItem() {
    // 先查看购物车
    NetworkMessage message(MessageType::CART_VIEW_REQUEST, "");

    waitingForResponse = true;
    if (!sendMessage(message)) {
        Utils::showError("发送购物车查看请求失败");
        Utils::pauseScreen();
        return;
    }

    UIManager::showLoading("正在加载购物车");

    // 等待响应
    while (waitingForResponse && connected) {
        Sleep(100);
    }

    if (!connected) return;

    if (currentCartItems.empty()) {
        Utils::showError("购物车为空！");
        Utils::pauseScreen();
        return;
    }

    UIManager::showUpdateCartItemForm();

    // 显示购物车商品
    std::cout << "购物车商品列表：" << std::endl;
    std::cout << "------------------------------------------------------------" << std::endl;

    for (size_t i = 0; i < currentCartItems.size(); ++i) {
        const auto& item = currentCartItems[i];
        std::cout << (i + 1) << ". " << item.name;

        if (item.discount < 1.0) {
            std::cout << " - " << Utils::formatMoney(item.currentPrice)
                << " (原价:" << Utils::formatMoney(item.originalPrice)
                << ", " << static_cast<int>(item.discount * 100) << "折)";
        }
        else {
            std::cout << " - " << Utils::formatMoney(item.currentPrice);
        }

        std::cout << " x" << item.quantity
            << " = " << Utils::formatMoney(item.getTotalPrice())
            << " [" << item.merchant << "] {" << item.type << "}" << std::endl;
    }

    std::cout << "------------------------------------------------------------" << std::endl;
    std::cout << std::endl;

    // 让用户选择要修改的商品
    std::cout << "请选择要修改的商品 (输入序号 1-" << currentCartItems.size() << "，0取消): ";
    std::string choiceStr;
    std::cin >> choiceStr;

    if (choiceStr == "0") {
        return;
    }

    try {
        int choice = std::stoi(choiceStr);
        if (choice < 1 || choice > static_cast<int>(currentCartItems.size())) {
            Utils::showError("无效的选择！");
            Utils::pauseScreen();
            return;
        }

        const auto& selectedItem = currentCartItems[choice - 1];

        std::cout << "当前数量: " << selectedItem.quantity << std::endl;
        std::cout << "请输入新的数量 (输入0删除商品): ";
        std::string quantityStr;
        std::cin >> quantityStr;

        try {
            int quantity = std::stoi(quantityStr);
            if (quantity < 0) {
                Utils::showError("数量不能为负数！");
                Utils::pauseScreen();
                return;
            }

            // 发送更新请求
            std::string data = std::to_string(selectedItem.id) + "|" + std::to_string(quantity);
            NetworkMessage updateMessage(MessageType::CART_UPDATE_ITEM_REQUEST, data);

            std::cout << "正在更新商品数量..." << std::endl;

            if (sendMessage(updateMessage)) {
                // 不等待响应，直接显示成功信息
                if (quantity == 0) {
                    Utils::showSuccess("商品已从购物车中移除");
                }
                else {
                    Utils::showSuccess("商品数量已更新为 " + std::to_string(quantity));
                }
                std::cout << "提示：请稍后查看购物车确认更新结果" << std::endl;
            }
            else {
                Utils::showError("发送更新请求失败");
            }
            Utils::pauseScreen();

        }
        catch (const std::exception& e) {
            Utils::showError("数量格式错误: " + std::string(e.what()));
            Utils::pauseScreen();
        }

    }
    catch (const std::exception& e) {
        Utils::showError("选择格式错误: " + std::string(e.what()));
        Utils::pauseScreen();
    }
}

void Client::handleRemoveCartItem() {
    // 先查看购物车
    NetworkMessage message(MessageType::CART_VIEW_REQUEST, "");

    waitingForResponse = true;
    if (!sendMessage(message)) {
        Utils::showError("发送购物车查看请求失败");
        Utils::pauseScreen();
        return;
    }

    UIManager::showLoading("正在加载购物车");

    // 等待响应
    while (waitingForResponse && connected) {
        Sleep(100);
    }

    if (!connected) return;

    if (currentCartItems.empty()) {
        Utils::showError("购物车为空！");
        Utils::pauseScreen();
        return;
    }

    UIManager::showRemoveCartItemForm();

    // 显示购物车商品
    std::cout << "购物车商品列表：" << std::endl;
    std::cout << "------------------------------------------------------------" << std::endl;

    for (size_t i = 0; i < currentCartItems.size(); ++i) {
        const auto& item = currentCartItems[i];
        std::cout << (i + 1) << ". " << item.name;

        if (item.discount < 1.0) {
            std::cout << " - " << Utils::formatMoney(item.currentPrice)
                << " (原价:" << Utils::formatMoney(item.originalPrice)
                << ", " << static_cast<int>(item.discount * 100) << "折)";
        }
        else {
            std::cout << " - " << Utils::formatMoney(item.currentPrice);
        }

        std::cout << " x" << item.quantity
            << " = " << Utils::formatMoney(item.getTotalPrice())
            << " [" << item.merchant << "] {" << item.type << "}" << std::endl;
    }

    std::cout << "------------------------------------------------------------" << std::endl;
    std::cout << std::endl;

    // 让用户选择要移除的商品
    std::cout << "请选择要移除的商品 (输入序号 1-" << currentCartItems.size() << "，0取消): ";
    std::string choiceStr;
    std::cin >> choiceStr;

    if (choiceStr == "0") {
        return;
    }

    try {
        int choice = std::stoi(choiceStr);
        if (choice < 1 || choice > static_cast<int>(currentCartItems.size())) {
            Utils::showError("无效的选择！");
            Utils::pauseScreen();
            return;
        }

        const auto& selectedItem = currentCartItems[choice - 1];

        // 确认删除
        std::cout << "确定要移除商品 \"" << selectedItem.name << "\" 吗？ (y/n): ";
        std::string confirm;
        std::cin >> confirm;

        if (confirm == "y" || confirm == "Y") {
            // 发送移除请求
            std::string data = std::to_string(selectedItem.id);
            NetworkMessage removeMessage(MessageType::CART_REMOVE_ITEM_REQUEST, data);

            std::cout << "正在移除商品..." << std::endl;

            if (sendMessage(removeMessage)) {
                // 不等待响应，直接显示成功信息
                Utils::showSuccess("商品已从购物车中移除");
                std::cout << "提示：请稍后查看购物车确认移除结果" << std::endl;
            }
            else {
                Utils::showError("发送移除请求失败");
            }
            Utils::pauseScreen();
        }

    }
    catch (const std::exception& e) {
        Utils::showError("选择格式错误: " + std::string(e.what()));
        Utils::pauseScreen();
    }
}

void Client::handleClearCart() {
    std::cout << "确定要清空整个购物车吗？此操作不可撤销！ (y/n): ";
    std::string confirm;
    std::cin >> confirm;

    if (confirm == "y" || confirm == "Y") {
        NetworkMessage message(MessageType::CART_CLEAR_REQUEST, "");

        std::cout << "正在清空购物车..." << std::endl;

        if (sendMessage(message)) {
            // 不等待响应，直接显示成功信息
            Utils::showSuccess("购物车已清空");
            std::cout << "提示：请稍后查看购物车确认清空结果" << std::endl;

            // 清空本地购物车缓存
            currentCartItems.clear();
            cartTotalPrice = 0.0;
            cartTotalCount = 0;
        }
        else {
            Utils::showError("发送清空购物车请求失败");
        }
        Utils::pauseScreen();
    }
}

void Client::handleViewOrders() {
    NetworkMessage message(MessageType::ORDER_LIST_REQUEST, "");

    waitingForResponse = true;
    if (sendMessage(message)) {
        UIManager::showLoading("正在加载订单列表");

        // 等待响应
        while (waitingForResponse && connected) {
            Sleep(100);
        }

        if (connected) {
            Utils::pauseScreen();
        }
    }
    else {
        Utils::showError("发送订单查询请求失败");
        Utils::pauseScreen();
    }
}

void Client::handleOrderManagement() {
    while (connected && !currentUser.empty()) {
        UIManager::showOrderMenu();

        std::string choice;
        std::cin >> choice;

        if (choice == "1") {
            handleViewOrders();
        }
        else if (choice == "2") {
            std::cout << "请输入订单ID: ";
            std::string orderIdStr;
            std::cin >> orderIdStr;
            try {
                int orderId = std::stoi(orderIdStr);
                handleOrderDetail(orderId);
            }
            catch (const std::exception& e) {
                Utils::showError("订单ID格式错误");
                Utils::pauseScreen();
            }
        }
        else if (choice == "3") {
            break;
        }
        else {
            Utils::showError("无效选择！");
            Utils::pauseScreen();
        }
    }
}

void Client::handleOrderDetail(int orderId) {
    std::string data = std::to_string(orderId);
    NetworkMessage message(MessageType::ORDER_DETAIL_REQUEST, data);

    std::cout << "正在加载订单详情..." << std::endl;

    if (sendMessage(message)) {
        // 不等待响应，直接显示提示
        Utils::showInfo("订单详情查看功能正在开发中，敬请期待");
        std::cout << "提示：您可以联系客服查询订单 #" << orderId << " 的详细信息" << std::endl;
    }
    else {
        Utils::showError("发送订单详情请求失败");
    }

    Utils::pauseScreen();
}