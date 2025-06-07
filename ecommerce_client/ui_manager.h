#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include <string>
#include <windows.h>

class UIManager {
public:
    // ��ʾϵͳ����
    static void showSystemTitle();

    // ��ʾ��ӭ��Ϣ
    static void showWelcomeMessage();

    // ��ʾ���˵�
    static void showMainMenu();

    // ��ʾ��¼��˵�
    static void showLoggedInMenu(const std::string& username, const std::string& userType, double balance);

    // ��ʾע�����
    static void showRegisterForm();

    // ��ʾ��¼����
    static void showLoginForm();

    // ��ʾ�˻�����˵�
    static void showAccountMenu();

    // ��ʾ�����޸Ľ���
    static void showChangePasswordForm();

    // ��ʾ��Ʒ�������
    static void showProductBrowseMenu();

    // ��ʾ��Ʒ�б�
    static void showProductList();

    // ��ʾ��Ʒ��������
    static void showProductSearchForm();

    // ��ʾ��Ʒ�������
    static void showProductDetail();

    // ��ʾ�̼���Ʒ����˵�
    static void showMerchantProductMenu();

    // ��ʾ�����Ʒ����
    static void showAddProductForm();

    // ��ʾ�޸���Ʒ����
    static void showModifyProductForm();

    // ��ʾ�̼���Ʒ�б�
    static void showMerchantProductList();

    // ��ʾ�����ۿ۽���
    static void showSetDiscountForm();

    // ��ʾ���ﳵ�˵��������ߣ�
    static void showCartMenu();

    // ��ʾ���ﳵ����
    static void showCartDetail();

    // ��ʾ�޸Ĺ��ﳵ��Ʒ��������
    static void showUpdateCartItemForm();

    // ��ʾ�Ƴ����ﳵ��Ʒ����
    static void showRemoveCartItemForm();

    // ��ʾ�����߲˵�
    static void showConsumerMenu();

    // ��ʾ��������˵�
    static void showOrderMenu();

    // ��ʾ�����б�
    static void showOrderList();

    // ��ʾ��������
    static void showOrderDetail();

    // ��ʾ�˳���Ϣ
    static void showExitMessage();

    // ��ȡ�˵�ѡ��
    static std::string getMenuChoice(int maxChoice);

    // ��ʾ���ض���
    static void showLoading(const std::string& message);

    // ��ʾ�������
    static void showCheckoutInterface();

    // ��ʾ����ȷ�Ͻ���
    static void showCheckoutConfirmation();
};

#endif