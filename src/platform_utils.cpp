#include "platform_utils.h"
#include <iostream>

#ifdef _WIN32
  #include <conio.h>
  #include <windows.h>
#else
  #include <termios.h>
  #include <unistd.h>
#endif

#include <csignal>
#include <functional>

// 静态成员初始化
std::function<void()> PlatformUtils::cleanupHandler;
std::function<void(int)> PlatformUtils::errorHandler;
bool PlatformUtils::isInitialized = false;

#ifndef _WIN32
struct termios PlatformUtils::originalTermios;
bool PlatformUtils::termiosModified = false;
#endif

std::string PlatformUtils::getHiddenInput() {
#ifdef _WIN32
    return getHiddenInputWindows();
#else
    return getHiddenInputUnix();
#endif
}

#ifdef _WIN32
std::string PlatformUtils::getHiddenInputWindows() {
    std::string password;
    char ch;
    while ((ch = _getch()) != '\r') {
        if (ch == '\b') {  // 退格键
            if (!password.empty()) {
                password.pop_back();
                std::cout << "\b \b";
            }
        } else {
            password += ch;
            std::cout << '*';
        }
    }
    std::cout << std::endl;
    return password;
}
#else
std::string PlatformUtils::getHiddenInputUnix() {
    struct termios old_settings, new_settings;
    tcgetattr(STDIN_FILENO, &old_settings);
    new_settings = old_settings;
    new_settings.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_settings);

    std::string password;
    char ch;
    while ((ch = getchar()) != '\n') {
        if (ch == 127 || ch == '\b') {  // 退格键
            if (!password.empty()) {
                password.pop_back();
                std::cout << "\b \b";
            }
        } else {
            password += ch;
            std::cout << '*';
        }
    }
    std::cout << std::endl;

    tcsetattr(STDIN_FILENO, TCSANOW, &old_settings);
    return password;
}
#endif

void PlatformUtils::clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void PlatformUtils::setupSignalHandlers() {
    if (isInitialized) return;
    
    #ifdef _WIN32
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    signal(SIGSEGV, signalHandler);
    signal(SIGABRT, signalHandler);
    #else
    struct sigaction sa;
    sa.sa_handler = signalHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    
    sigaction(SIGINT, &sa, nullptr);   // Ctrl+C
    sigaction(SIGTERM, &sa, nullptr);  // 终止信号
    sigaction(SIGSEGV, &sa, nullptr);  // 段错误
    sigaction(SIGABRT, &sa, nullptr);  // 异常终止
    sigaction(SIGPIPE, &sa, nullptr);  // 管道破裂
    #endif
    
    #ifndef _WIN32
    // 保存原始终端设置
    if (tcgetattr(STDIN_FILENO, &originalTermios) == 0) {
        termiosModified = true;
    }
    #endif
    
    isInitialized = true;
}

void PlatformUtils::signalHandler(int signum) {
    std::cout << "\n接收到信号 " << signum << std::endl;
    
    switch (signum) {
        case SIGINT:
            std::cout << "\n程序被中断，正在保存数据...\n";
            break;
        case SIGTERM:
            std::cout << "\n程序被终止，正在清理...\n";
            break;
        case SIGSEGV:
            std::cout << "\n发生段错误，尝试保存数据...\n";
            break;
        case SIGABRT:
            std::cout << "\n程序异常终止，执行紧急清理...\n";
            break;
        default:
            std::cout << "\n未知信号，执行安全退出...\n";
    }
    
    emergencyCleanup();
    exit(signum);
}

void PlatformUtils::emergencyCleanup() {
    try {
        if (cleanupHandler) {
            cleanupHandler();
        }
        restoreTerminal();
        
        // 确保数据库连接被正确关闭
        try {
            DatabaseManager::getInstance()->close();
        } catch (...) {
            // 忽略清理过程中的错误
        }
        
        std::cout << "清理完成\n";
    } catch (...) {
        std::cerr << "清理过程中发生错误\n";
    }
}

void PlatformUtils::registerCleanupHandler(std::function<void()> handler) {
    cleanupHandler = handler;
}

void PlatformUtils::registerErrorHandler(std::function<void(int)> handler) {
    errorHandler = handler;
}

void PlatformUtils::restoreTerminal() {
    #ifndef _WIN32
    if (termiosModified) {
        tcsetattr(STDIN_FILENO, TCSANOW, &originalTermios);
        termiosModified = false;
    }
    #endif
}

void PlatformUtils::cleanup() {
    if (cleanupHandler) {
        try {
            cleanupHandler();
        } catch (const std::exception& e) {
            std::cerr << "清理时发生错误: " << e.what() << std::endl;
        }
    }
    restoreTerminal();
}

void PlatformUtils::handleError(const std::string& errorMsg) {
    std::cerr << "错误: " << errorMsg << std::endl;
    if (errorHandler) {
        try {
            errorHandler(1);
        } catch (...) {
            // 忽略错误处理器中的错误
        }
    }
    cleanup();
}