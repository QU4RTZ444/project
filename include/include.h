/*
@file include.h
@author QU4RTZ444
@date 2025-04-17 17:15
 */
#ifndef include_H
#define include_H

// C++ 标准库头文件
#include <iostream>
#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
#include <fstream>
#include <functional>
#include <limits>
#include <stdexcept>
#include <sstream>
#include <chrono>
#include <ctime>
#include <filesystem>

// 第三方库头文件
#include <sqlite3.h>

// 项目基础工具类
#include "platform_utils.h"

// 核心数据类
#include "product.h"
#include "user.h"
#include "cart_item.h"
#include "shopping_cart.h"
#include "order.h"

// 数据管理类
#include "database_manager.h"
#include "user_manager.h"
#include "product_manager.h"
#include "order_manager.h"

// 界面管理类
#include "menu.h"
#include "menu_handler.h"

// 添加新的常量定义
const int ORDER_TIMEOUT = 1800;  // 订单超时时间(秒)
const int MAX_CART_ITEMS = 10;   // 购物车最大商品数

#ifndef DEBUG
//#define DEBUG  // 注释掉以禁用调试信息
#endif

#ifdef DEBUG
#define DEBUG_LOG(x) std::cout << "[DEBUG] " << x << "\n"
#else
#define DEBUG_LOG(x)
#endif

#endif // include_H