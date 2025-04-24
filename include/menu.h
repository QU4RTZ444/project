/*
@file menu.h
@author QU4RTZ444
@date 2025-04-24 23:22
 */
#ifndef menu_H
#define menu_H

#include "include.h"

class Menu {
private:
    // 显示系统主菜单界面
    static void showMainMenu();
    
    // 显示消费者专属菜单
    static void showConsumerMenu();
    
    // 显示商家专属菜单
    static void showSellerMenu();
    
    // 显示商品管理菜单
    static void showProductMenu();
    
    // 清理控制台屏幕，实现跨平台支持
    static void clearScreen();

public:
    // 显示主菜单并处理用户输入
    static void displayMainMenu();
    
    // 根据用户类型显示对应的菜单
    // @param currentUser: 当前登录用户的共享指针
    static void displayUserMenu(const std::shared_ptr<User>& currentUser);
    
    // 获取用户输入的选项
    // @param min: 最小有效值
    // @param max: 最大有效值
    // @return: 返回用户输入的有效选项
    static int getChoice(int min, int max);
};

#endif // menu_H
