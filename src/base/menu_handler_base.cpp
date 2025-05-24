#include "include.h"

void MenuHandlerBase::waitForKey() const {
    std::cout << "\n按回车键继续...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

bool MenuHandlerBase::confirmAction(const std::string& message) const {
    std::cout << message << " (1:是 0:否): ";
    int confirm;
    if (std::cin >> confirm) {
        return confirm == 1;
    }
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return false;
}