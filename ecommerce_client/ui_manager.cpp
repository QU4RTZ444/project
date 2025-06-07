#include "ui_manager.h"
#include "utils.h"
#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>

void UIManager::showSystemTitle() {
    Utils::clearScreen();
    std::cout << "\n";
    std::cout << "    ================================================\n";
    std::cout << "    |                                              |\n";
    std::cout << "    |              ���̽���ƽ̨ϵͳ                  |\n";
    std::cout << "    |           E-Commerce Platform                |\n";
    std::cout << "    |                                              |\n";
    std::cout << "    |                �汾 1.0                     |\n";
    std::cout << "    |                                              |\n";
    std::cout << "    ================================================\n";
    std::cout << std::endl;
}

void UIManager::showWelcomeMessage() {
    showSystemTitle();
    Utils::showInfo("��ӭʹ�õ��̽���ƽ̨��");
    Utils::showInfo("ϵͳ�������ӷ�����...");
    Sleep(1000); // Windows Sleep����
}

void UIManager::showMainMenu() {
    Utils::clearScreen();
    showSystemTitle();
    Utils::showSeparator("���˵�");

    std::cout << "    +-------------------------------------+\n";
    std::cout << "    |  1. �û�ע��                        |\n";
    std::cout << "    |  2. �û���¼                        |\n";
    std::cout << "    |  3. �����Ʒ                        |\n";
    std::cout << "    |  4. �˳�ϵͳ                        |\n";
    std::cout << "    +-------------------------------------+\n";
    std::cout << std::endl;
}

void UIManager::showProductBrowseMenu() {
    Utils::clearScreen();
    showSystemTitle();
    Utils::showSeparator("��Ʒ���");

    std::cout << "    +-------------------------------------+\n";
    std::cout << "    |  1. �鿴��Ʒ�б�                    |\n";
    std::cout << "    |  2. ������Ʒ                        |\n";
    std::cout << "    |  3. �������˵�                      |\n";
    std::cout << "    +-------------------------------------+\n";
    std::cout << std::endl;
}

void UIManager::showProductList() {
    Utils::clearScreen();
    showSystemTitle();
    Utils::showSeparator("��Ʒ�б�");
}

void UIManager::showProductSearchForm() {
    Utils::clearScreen();
    showSystemTitle();
    Utils::showSeparator("��Ʒ����");

    std::cout << "�����������ؼ��ʣ�" << std::endl;
    std::cout << std::endl;
}

void UIManager::showProductDetail() {
    Utils::clearScreen();
    showSystemTitle();
    Utils::showSeparator("��Ʒ����");
}

void UIManager::showLoggedInMenu(const std::string& username, const std::string& userType, double balance) {
    Utils::clearScreen();
    showSystemTitle();
    Utils::showSeparator("�û�����");

    std::cout << "    +-------------------------------------+\n";
    std::cout << "    |  �û�: " << std::left << std::setw(28) << username << "|\n";
    std::cout << "    |  ����: " << std::left << std::setw(28) << userType << "|\n";
    std::cout << "    |  ���: " << std::left << std::setw(28) << Utils::formatMoney(balance) << "|\n";
    std::cout << "    +-------------------------------------+\n";
    std::cout << std::endl;

    std::cout << "    +-------------------------------------+\n";
    std::cout << "    |  1. �鿴��Ʒ                        |\n";
    std::cout << "    |  2. ������Ʒ                        |\n";

    if (userType == "������") {
        std::cout << "    |  3. ���ﳵ����                      |\n";
    }
    else {
        std::cout << "    |  3. ��Ʒ����                        |\n";
    }

    std::cout << "    |  4. ��������                        |\n";  // �̼Һ������߶��ж�������
    std::cout << "    |  5. �˻�����                        |\n";
    std::cout << "    |  6. �û��ǳ�                        |\n";
    std::cout << "    +-------------------------------------+\n";
    std::cout << std::endl;
}

void UIManager::showRegisterForm() {
    Utils::clearScreen();
    showSystemTitle();
    Utils::showSeparator("�û�ע��");

    std::cout << "����дע����Ϣ��" << std::endl;
    std::cout << std::endl;
}

void UIManager::showLoginForm() {
    Utils::clearScreen();
    showSystemTitle();
    Utils::showSeparator("�û���¼");

    std::cout << "�������¼��Ϣ��" << std::endl;
    std::cout << std::endl;
}

void UIManager::showMerchantProductMenu() {
    Utils::clearScreen();
    showSystemTitle();
    Utils::showSeparator("��Ʒ����");

    std::cout << "    +-------------------------------------+\n";
    std::cout << "    |  1. �����Ʒ                        |\n";
    std::cout << "    |  2. �鿴�ҵ���Ʒ                    |\n";
    std::cout << "    |  3. �޸���Ʒ��Ϣ                    |\n";
    std::cout << "    |  4. ������Ʒ�ۿ�                    |\n";
    std::cout << "    |  5. �������˵�                      |\n";
    std::cout << "    +-------------------------------------+\n";
    std::cout << std::endl;
}

void UIManager::showAddProductForm() {
    Utils::clearScreen();
    showSystemTitle();
    Utils::showSeparator("�����Ʒ");

    std::cout << "����д��Ʒ��Ϣ��" << std::endl;
    std::cout << "֧�ֵ���Ʒ���ͣ�" << std::endl;
    std::cout << "  1. ʳƷ" << std::endl;
    std::cout << "  2. �鼮" << std::endl;
    std::cout << "  3. �·�" << std::endl;
    std::cout << std::endl;
}

void UIManager::showModifyProductForm() {
    Utils::clearScreen();
    showSystemTitle();
    Utils::showSeparator("�޸���Ʒ��Ϣ");

    std::cout << "����·���Ʒ�б���ѡ��Ҫ�޸ĵ���Ʒ��" << std::endl;
    std::cout << "��ʾ������Ҫ�޸ĵ��ֶο���ֱ�Ӱ��س�����" << std::endl;
    std::cout << std::endl;
}

void UIManager::showMerchantProductList() {
    Utils::clearScreen();
    showSystemTitle();
    Utils::showSeparator("�ҵ���Ʒ");
}

void UIManager::showSetDiscountForm() {
    Utils::clearScreen();
    showSystemTitle();
    Utils::showSeparator("������Ʒ�ۿ�");

    std::cout << "��ѡ���ۿ����÷�ʽ��" << std::endl;
    std::cout << "�ۿ۷�Χ��0.1 - 1.0 (����0.8��ʾ8��)" << std::endl;
    std::cout << std::endl;
}

void UIManager::showConsumerMenu() {
    Utils::clearScreen();
    showSystemTitle();
    Utils::showSeparator("�����߹��ܲ˵�");

    std::cout << "    +-------------------------------------+\n";
    std::cout << "    |  1. ��Ʒ���                        |\n";
    std::cout << "    |  2. ��Ʒ����                        |\n";
    std::cout << "    |  3. ���ﳵ����                      |\n";
    std::cout << "    |  4. ��������                        |\n";  // ��Ӷ�������
    std::cout << "    |  5. �޸�����                        |\n";
    std::cout << "    |  6. �˳���¼                        |\n";
    std::cout << "    +-------------------------------------+\n";
    std::cout << "\n    ��ѡ�� (1-6): ";
}

void UIManager::showCartMenu() {
    Utils::clearScreen();
    showSystemTitle();
    Utils::showSeparator("���ﳵ����");

    std::cout << "    +-------------------------------------+\n";
    std::cout << "    |  1. �鿴���ﳵ                      |\n";
    std::cout << "    |  2. �޸���Ʒ����                    |\n";
    std::cout << "    |  3. �Ƴ���Ʒ                        |\n";
    std::cout << "    |  4. ��չ��ﳵ                      |\n";
    std::cout << "    |  5. ���㹺�ﳵ                      |\n";
    std::cout << "    |  6. �����ϼ��˵�                    |\n";
    std::cout << "    +-------------------------------------+\n";
    std::cout << "\n    ��ѡ�� (1-6): ";
}

void UIManager::showCheckoutInterface() {
    Utils::clearScreen();
    showSystemTitle();
    Utils::showSeparator("���ﳵ����");

    std::cout << "����׼��������Ϣ..." << std::endl;
}

void UIManager::showCheckoutConfirmation() {
    Utils::clearScreen();
    showSystemTitle();
    Utils::showSeparator("ȷ�϶���");

    std::cout << "��ȷ�����¶�����Ϣ��" << std::endl;
    std::cout << "============================================================" << std::endl;
}

void UIManager::showOrderMenu() {
    Utils::clearScreen();
    showSystemTitle();
    Utils::showSeparator("��������");

    std::cout << "    +-------------------------------------+\n";
    std::cout << "    |  1. �鿴�����б�                    |\n";
    std::cout << "    |  2. �鿴��������                    |\n";
    std::cout << "    |  3. �����ϼ��˵�                    |\n";
    std::cout << "    +-------------------------------------+\n";
    std::cout << "\n    ��ѡ�� (1-3): ";
}

void UIManager::showOrderList() {
    Utils::clearScreen();
    showSystemTitle();
    Utils::showSeparator("�����б�");

    std::cout << std::endl;
}

void UIManager::showOrderDetail() {
    Utils::clearScreen();
    showSystemTitle();
    Utils::showSeparator("��������");

    std::cout << "������ϸ��Ϣ��" << std::endl;
    std::cout << std::endl;
}

void UIManager::showAccountMenu() {
    Utils::clearScreen();
    showSystemTitle();
    Utils::showSeparator("�˻�����");

    std::cout << "    +-------------------------------------+\n";
    std::cout << "    |  1. �޸�����                        |\n";
    std::cout << "    |  2. �鿴�˻���Ϣ                    |\n";
    std::cout << "    |  3. �˻���ֵ (������)               |\n";
    std::cout << "    |  4. �˻����� (������)               |\n";
    std::cout << "    |  5. �������˵�                      |\n";
    std::cout << "    +-------------------------------------+\n";
    std::cout << std::endl;
}

void UIManager::showChangePasswordForm() {
    Utils::clearScreen();
    showSystemTitle();
    Utils::showSeparator("�޸�����");

    std::cout << "������������Ϣ��" << std::endl;
    std::cout << std::endl;
}

std::string UIManager::getMenuChoice(int maxChoice) {
    std::string choice;
    while (true) {
        std::cout << "��ѡ����� (1-" << maxChoice << "): ";
        std::cin >> choice;

        if (!choice.empty() && choice.length() == 1 &&
            choice[0] >= '1' && choice[0] <= ('0' + maxChoice)) {
            return choice;
        }

        Utils::showError("��Чѡ�����������룡");
    }
}

void UIManager::showLoading(const std::string& message) {
    std::cout << message;
    for (int i = 0; i < 3; ++i) {
        std::cout << ".";
        std::cout.flush();
        Sleep(300); // Windows Sleep����
    }
    std::cout << std::endl;
}

void UIManager::showExitMessage() {
    Utils::clearScreen();
    showSystemTitle();
    std::cout << "\nллʹ�õ��̽���ƽ̨��\n" << std::endl;
    std::cout << "�ټ���\n" << std::endl;
}

void UIManager::showCartDetail() {
    Utils::clearScreen();
    showSystemTitle();
    Utils::showSeparator("���ﳵ����");

    std::cout << "���Ĺ��ﳵ�������£�" << std::endl;
    std::cout << std::endl;
}

void UIManager::showUpdateCartItemForm() {
    Utils::clearScreen();
    showSystemTitle();
    Utils::showSeparator("�޸���Ʒ����");

    std::cout << "��ѡ��Ҫ�޸���������Ʒ��" << std::endl;
    std::cout << "��ʾ������0����ɾ������Ʒ" << std::endl;
    std::cout << std::endl;
}

void UIManager::showRemoveCartItemForm() {
    Utils::clearScreen();
    showSystemTitle();
    Utils::showSeparator("�Ƴ����ﳵ��Ʒ");

    std::cout << "��ѡ��Ҫ�Ƴ�����Ʒ��" << std::endl;
    std::cout << "��ʾ���˲�������ȫ�Ƴ�����Ʒ" << std::endl;
    std::cout << std::endl;
}