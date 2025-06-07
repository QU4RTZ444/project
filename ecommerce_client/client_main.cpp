#include "client.h"
#include <iostream>
#include <string>

int main() {
    std::cout << "=== ���̽���ƽ̨�ͻ��� ===" << std::endl;
    std::cout << "�������ӷ�����..." << std::endl;

    Client client;

    // ���ӵ�������
    std::string serverIP = "127.0.0.1";
    int port = 8080;

    std::cout << "�������ӵ������� " << serverIP << ":" << port << std::endl;

    if (!client.connectToServer(serverIP, port)) {
        std::cerr << "���ӷ�����ʧ��!" << std::endl;
        std::cerr << "��ȷ���������������������˿� " << port << std::endl;
        std::cout << "��Enter���˳�..." << std::endl;
        std::cin.get();
        return -1;
    }

    std::cout << "�ɹ����ӵ�������!" << std::endl;
    std::cout << "��ӭʹ�õ��̽���ƽ̨!" << std::endl;

    try {
        // ���пͻ���
        client.run();
    }
    catch (const std::exception& e) {
        std::cerr << "�ͻ�������ʱ��������: " << e.what() << std::endl;
    }

    std::cout << "�ͻ������˳�" << std::endl;
    return 0;
}