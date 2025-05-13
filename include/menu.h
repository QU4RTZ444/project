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
    public:
    /**
     * @brief 显示主菜单界面
     */
    static void displayMainMenu();

    /**
     * @brief 根据用户类型显示对应的用户菜单
     * @param currentUser 当前登录的用户对象
     */
    static void displayUserMenu(const std::shared_ptr<User>& currentUser);

    /**
     * @brief 显示主菜单选项
     */
    static void showMainMenu();

    /**
     * @brief 显示消费者菜单选项
     */
    static void showConsumerMenu();

    /**
     * @brief 显示商家菜单选项
     */
    static void showSellerMenu();

    /**
     * @brief 获取用户输入的选项
     * @param min 选项的最小值
     * @param max 选项的最大值
     * @return int 返回用户选择的选项
     */
    static int getChoice(int min, int max);

    /**
     * @brief 清空控制台屏幕
     */
    static void clearScreen();
};

#endif // MENU_H
