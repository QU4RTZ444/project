#ifndef UTILS_H
#define UTILS_H

#define NOMINMAX
#include <string>
#include <limits>

// Windowsר�ù��ߺ���
class Utils {
public:
    // ��������
    static void clearScreen();

    // ��ͣ�ȴ��û�����
    static void pauseScreen();

    // ��ʾ�ָ���
    static void showSeparator(const std::string& title = "");

    // ��ʽ��������
    static std::string formatMoney(double amount);

    // ��ȡ�û����루����ʾ��
    static std::string getInput(const std::string& prompt);

    // ��ʾ�ɹ���Ϣ
    static void showSuccess(const std::string& message);

    // ��ʾ������Ϣ
    static void showError(const std::string& message);

    // ��ʾ��Ϣ��Ϣ
    static void showInfo(const std::string& message);
};

#endif