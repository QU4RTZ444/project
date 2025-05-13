#include "include.h"

int main() {
    try {
        // 显示启动信息
        std::cout << "正在启动系统...\n";
        
        // 初始化数据库
        std::cout << "正在初始化数据库...\n";
        DatabaseManager::getInstance()->init();
        
        // 设置信号处理
        PlatformUtils::setupSignalHandlers();
        
        // 注册清理函数 - 只负责关闭数据库
        PlatformUtils::registerCleanupHandler([]() {
            std::cout << "正在关闭数据库连接...\n";
            try {
                DatabaseManager::getInstance()->close();
            } catch (const std::exception& e) {
                std::cerr << "关闭数据库时出错: " << e.what() << std::endl;
            }
        });
        
        // 注册错误处理函数 - 简化错误日志记录
        PlatformUtils::registerErrorHandler([](int errorCode) {
            std::cerr << "程序发生错误(代码:" << errorCode << ")\n";
        });
        
        // 显示欢迎信息
        std::cout << "\n欢迎使用电商平台 V1 - 单机版账户与商品管理子系统\n";
        std::cout << "Copyright © 2025 QU4RTZ. All rights reserved.\n\n";
        
        // 初始化管理器并显示主菜单
        std::cout << "正在初始化系统组件...\n";
        UserManager userManager;
        ProductManager productManager;
        Menu m;

        PlatformUtils::clearScreen();
        m.displayMainMenu();

    } catch (const std::exception& e) {
        std::cerr << "程序发生严重错误: " << e.what() << std::endl;
        return 1;
    }
    
    PlatformUtils::cleanup();
    std::cout << "感谢使用，再见！\n";
    return 0;
}
