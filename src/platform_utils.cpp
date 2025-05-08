#include "platform_utils.h"
#include <iostream>

#ifdef _WIN32
  #include <conio.h>
  #include <windows.h>
#else
  #include <termios.h>
  #include <unistd.h>
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