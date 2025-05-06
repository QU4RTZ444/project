#include "include.h"
#include "menu.h"

void Menu::showMainMenu() {
  clearScreen();
  std::cout << "\n┌────────────────────────────────┐\n";
  std::cout << "│        电商交易平台主菜单      │\n";
  std::cout << "├────────────────────────────────┤\n";
  std::cout << "│  1. 用户登录                   │\n";
  std::cout << "│  2. 用户注册                   │\n";
  std::cout << "│  3. 浏览所有商品              │\n";
  std::cout << "│  0. 退出系统                   │\n";
  std::cout << "└────────────────────────────────┘\n";
}

void Menu::displayMainMenu() {
  while(true){
    showMainMenu();

    int choice = getChoice(0, 3);

    switch(choice){
      case 0://退出
        std::cout<<"感谢使用，再见！\n";
        return;

      case 1://用户登录
        break;

      case 2://用户注册
        break;

      case 3://浏览商品
        break;
    }
  }
}

void Menu::displayUserMenu(const std::shared_ptr<User>& currentUser) {
  clearScreen();
  if (currentUser->getUserType() == "消费者") {
      showConsumerMenu();
  } else if (currentUser->getUserType() == "商家") {
      showSellerMenu();
  }
}

void Menu::showConsumerMenu() {
  std::cout << "\n==== 消费者菜单 ====\n";
  std::cout << "1. 查看商品列表\n";
  std::cout << "2. 搜索商品\n";
  std::cout << "3. 查看购物车\n";
  std::cout << "4. 查看订单历史\n";
  std::cout << "5. 账户余额管理\n";
  std::cout << "6. 修改密码\n";
  std::cout << "0. 退出登录\n";
  std::cout << "请选择操作 [0-6]: ";
}

void Menu::showSellerMenu() {
  std::cout << "\n==== 商家菜单 ====\n";
  std::cout << "1. 添加新商品\n";
  std::cout << "2. 管理商品\n";
  std::cout << "3. 查看销售统计\n";
  std::cout << "4. 账户余额查询\n";
  std::cout << "5. 修改密码\n";
  std::cout << "0. 退出登录\n";
  std::cout << "请选择操作 [0-5]: ";
}

int Menu::getChoice(int min, int max) {
  int choice;
  while (true) {
      if (std::cin >> choice && choice >= min && choice <= max) {
          return choice;
      }
      std::cin.clear();
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      std::cout << "输入无效，请重新输入 [" << min << "-" << max << "]: ";
  }
}

void Menu::clearScreen() {
  #ifdef _WIN32
      system("cls");
  #else
      system("clear");
  #endif
}