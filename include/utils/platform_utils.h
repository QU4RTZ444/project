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

/**
 * @brief 平台工具类
 * @details 提供跨平台的工具函数，包括终端输入输出控制、信号处理和错误处理
 */
class PlatformUtils {
public:
    /**
     * @brief 获取隐藏输入（如密码）
     * @return string 用户输入的字符串
     */
    static std::string getHiddenInput();

    /**
     * @brief 清空终端屏幕
     */
    static void clearScreen();
    
    /**
     * @brief 设置信号处理器
     * @details 设置程序对SIGINT、SIGTERM等信号的处理
     */
    static void setupSignalHandlers();

    /**
     * @brief 注册清理处理函数
     * @param handler 清理函数
     * @details 注册程序退出时需要执行的清理工作
     */
    static void registerCleanupHandler(std::function<void()> handler);

    /**
     * @brief 恢复终端设置
     * @details 恢复终端到程序开始前的状态
     */
    static void restoreTerminal();

    /**
     * @brief 执行清理工作
     * @details 调用已注册的清理处理函数
     */
    static void cleanup();
    
    /**
     * @brief 注册错误处理函数
     * @param handler 错误处理函数
     * @details 注册处理程序运行时错误的函数
     */
    static void registerErrorHandler(std::function<void(int)> handler);

    /**
     * @brief 处理错误消息
     * @param errorMsg 错误信息
     * @details 处理并记录错误信息
     */
    static void handleError(const std::string& errorMsg);
    
private:
    #ifdef _WIN32
    /**
     * @brief Windows平台下获取隐藏输入
     * @return string 用户输入的字符串
     */
    static std::string getHiddenInputWindows();
    #else
    /**
     * @brief Unix平台下获取隐藏输入
     * @return string 用户输入的字符串
     */
    static std::string getHiddenInputUnix();

    /** @brief 保存原始终端设置 */
    static struct termios originalTermios;

    /** @brief 终端设置是否已修改的标志 */
    static bool termiosModified;
    #endif
    
    /** @brief 清理处理函数 */
    static std::function<void()> cleanupHandler;

    /** @brief 错误处理函数 */
    static std::function<void(int)> errorHandler;

    /** @brief 类是否已初始化的标志 */
    static bool isInitialized;
    
    /**
     * @brief 信号处理函数
     * @param signum 信号编号
     * @details 处理程序接收到的系统信号
     */
    static void signalHandler(int signum);

    /**
     * @brief 紧急清理函数
     * @details 在程序异常终止时执行必要的清理工作
     */
    static void emergencyCleanup();
};

#endif // platform_utils_H