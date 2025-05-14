/*
@file include.h
@author QU4RTZ444
@date 2025-04-17 17:15
 */
#ifndef include_H
#define include_H

#include <iostream>
#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
#include <fstream>
#include <functional>
#include <limits>
#include <sqlite3.h>
#include <stdexcept>
#include <sstream>
#include <chrono>
#include <ctime>
#include <filesystem>

// 先包含基础类
#include "database_manager.h"
#include "user.h"
#include "product.h"
#include "platform_utils.h"
#include "menu_handler.h"
#include "order.h"
#include "shopping_cart.h"

// 再包含管理器类
#include "user_manager.h"
#include "product_manager.h"
#include "menu.h"
#include "order_manager.h"

// 添加新的常量定义
const int ORDER_TIMEOUT = 1800;  // 订单超时时间(秒)
const int MAX_CART_ITEMS = 10;   // 购物车最大商品数

#endif // include_H