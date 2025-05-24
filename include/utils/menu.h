/*
@file menu.h
@author QU4RTZ444
@date 2025-04-24 23:22
 */
#ifndef MENU_H
#define MENU_H

#include "include.h"
#include "menu_handler.h"

class Menu {
private:
    class MenuImpl;  // 前向声明

public:
    /**
     * @brief 构造函数
     * @param um 用户管理器
     * @param pm 商品管理器
     */
    Menu(UserManager& um, ProductManager& pm);
    
    /**
     * @brief 析构函数
     */
    ~Menu();

    /**
     * @brief 显示并运行主菜单
     */
    void run();

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

    /**
     * @brief 显示主菜单界面
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

private:
    std::unique_ptr<MenuImpl> pimpl; // PIMPL 实现
};

#endif
