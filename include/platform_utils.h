/*
@file platform_utils.h
@author QU4RTZ444
@date 2025-05-08 15:15
 */
#ifndef platform_utils_H
#define platform_utils_H

#include "include.h"

class PlatformUtils {
  public:
      // 获取密码（隐藏输入）
      static std::string getHiddenInput();
      
      // 清理屏幕
      static void clearScreen();
      
  private:
  #ifdef _WIN32
      static std::string getHiddenInputWindows();
  #else
      static std::string getHiddenInputUnix();
  #endif
};

#endif // platform_utils_H