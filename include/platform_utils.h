/*
@file platform_utils.h
@author QU4RTZ444
@date 2025-05-08 15:15
 */
#ifndef platform_utils_H
#define platform_utils_H

#include "include.h"
#include <csignal>
#include <functional>

class PlatformUtils {
public:
    // 现有功能
    static std::string getHiddenInput();
    static void clearScreen();
    
    // 新增信号处理相关功能
    static void setupSignalHandlers();
    static void registerCleanupHandler(std::function<void()> handler);
    static void restoreTerminal();
    static void cleanup();
    
    // 新增错误处理和资源清理功能
    static void registerErrorHandler(std::function<void(int)> handler);
    static void handleError(const std::string& errorMsg);
    
private:
    #ifdef _WIN32
    static std::string getHiddenInputWindows();
    #else
    static std::string getHiddenInputUnix();
    static struct termios originalTermios;
    static bool termiosModified;
    #endif
    
    static std::function<void()> cleanupHandler;
    static std::function<void(int)> errorHandler;
    static bool isInitialized;
    
    // 信号处理函数
    static void signalHandler(int signum);
    static void emergencyCleanup();
};

#endif // platform_utils_H