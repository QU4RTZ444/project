#include "client.h"
#include <iostream>
#include <string>

int main() {
    std::cout << "=== 电商交易平台客户端 ===" << std::endl;
    std::cout << "正在连接服务器..." << std::endl;

    Client client;

    // 连接到服务器
    std::string serverIP = "127.0.0.1";
    int port = 8080;

    std::cout << "尝试连接到服务器 " << serverIP << ":" << port << std::endl;

    if (!client.connectToServer(serverIP, port)) {
        std::cerr << "连接服务器失败!" << std::endl;
        std::cerr << "请确保服务器已启动并监听端口 " << port << std::endl;
        std::cout << "按Enter键退出..." << std::endl;
        std::cin.get();
        return -1;
    }

    std::cout << "成功连接到服务器!" << std::endl;
    std::cout << "欢迎使用电商交易平台!" << std::endl;

    try {
        // 运行客户端
        client.run();
    }
    catch (const std::exception& e) {
        std::cerr << "客户端运行时发生错误: " << e.what() << std::endl;
    }

    std::cout << "客户端已退出" << std::endl;
    return 0;
}