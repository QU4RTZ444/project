#ifndef UTILS_H
#define UTILS_H

#define NOMINMAX
#include <string>
#include <limits>

// Windows专用工具函数
class Utils {
public:
    // 清屏函数
    static void clearScreen();

    // 暂停等待用户输入
    static void pauseScreen();

    // 显示分隔线
    static void showSeparator(const std::string& title = "");

    // 格式化输出金额
    static std::string formatMoney(double amount);

    // 获取用户输入（带提示）
    static std::string getInput(const std::string& prompt);

    // 显示成功消息
    static void showSuccess(const std::string& message);

    // 显示错误消息
    static void showError(const std::string& message);

    // 显示信息消息
    static void showInfo(const std::string& message);
};

#endif