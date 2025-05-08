/*
@file menu.h
@author QU4RTZ444
@date 2025-04-24 23:22
 */
#ifndef MENU_H
#define MENU_H

#include "include.h"

class Menu {
private:
    class MenuImpl;  // 前向声明

public:
    static void displayMainMenu();
    static void displayUserMenu(const std::shared_ptr<User>& currentUser);
    static void showMainMenu();
    static void showConsumerMenu();
    static void showSellerMenu();
    static int getChoice(int min, int max);
    static void clearScreen();
};

#endif // MENU_H
