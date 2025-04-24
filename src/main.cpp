#include "include.h"

int main() {
    std::cout << "欢迎使用电商平台 V1 - 单机版账户与商品管理子系统\n";
    // 初始化管理器
    UserManager userManager;
    ProductManager productManager;

    // 加载用户与商品数据
    userManager.loadFromFile();
    productManager.loadFromFile();

    // TODO: 添加菜单系统和交互逻辑

    return 0;
}
