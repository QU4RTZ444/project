#include "utils.h"
#include <iostream>
#include <cstdlib>
#include <iomanip>
#include <sstream>
#include <windows.h>
#include <conio.h>

void Utils::clearScreen() {
    system("cls");
}

void Utils::pauseScreen() {
    std::cout << "\n按任意键继续...";
    _getch();
    std::cout << std::endl;
}

void Utils::showSeparator(const std::string& title) {
    std::cout << "\n";
    if (title.empty()) {
        std::cout << "================================================" << std::endl;
    }
    else {
        int titleLen = static_cast<int>(title.length());
        int totalWidth = 48;
        int sideWidth = (totalWidth - titleLen - 2) / 2;

        std::cout << std::string(sideWidth, '=') << " " << title << " "
            << std::string(totalWidth - sideWidth - titleLen - 2, '=') << std::endl;
    }
    std::cout << std::endl;
}

std::string Utils::formatMoney(double amount) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << amount;
    return oss.str() + " 元";
}

std::string Utils::getInput(const std::string& prompt) {
    std::string input;
    std::cout << prompt;
    std::cin >> input;
    //std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return input;
}

void Utils::showSuccess(const std::string& message) {
    std::cout << "\n[成功] " << message << std::endl;
}

void Utils::showError(const std::string& message) {
    std::cout << "\n[错误] " << message << std::endl;
}

void Utils::showInfo(const std::string& message) {
    std::cout << "\n[信息] " << message << std::endl;
}