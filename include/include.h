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

// 先包含基础类
#include "database_manager.h"
#include "user.h"
#include "product.h"

// 再包含管理器类
#include "user_manager.h"
#include "product_manager.h"
#include "menu.h"

#endif // include_H