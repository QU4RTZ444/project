#include "server.h"
#include <iostream>
#include <string>
#include <thread>

int main() {
    std::cout << "=== 电商交易平台服务器 ===" << std::endl;
    std::cout << "正在初始化服务器..." << std::endl;

    Server server(8080);

    if (!server.start()) {
        std::cerr << "服务器启动失败!" << std::endl;
        std::cout << "请检查端口8080是否被占用" << std::endl;
        return -1;
    }

    std::cout << "服务器启动成功! 按Enter键停止服务器" << std::endl;
    std::cout << "服务器监听端口: 8080" << std::endl;
    std::cout << "等待客户端连接..." << std::endl;

    // 启动一个线程运行服务器
    std::thread serverThread(&Server::run, &server);

    //server.run();

    // 等待用户输入来停止服务器
    std::string input;
    std::getline(std::cin, input);

    std::cout << "正在停止服务器..." << std::endl;
    server.stop();

    if (serverThread.joinable()) {
        serverThread.join();
    }

    std::cout << "服务器已停止" << std::endl;
    return 0;
}