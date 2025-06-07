#include "server.h"
#include <iostream>
#include <string>
#include <thread>

int main() {
    std::cout << "=== ���̽���ƽ̨������ ===" << std::endl;
    std::cout << "���ڳ�ʼ��������..." << std::endl;

    Server server(8080);

    if (!server.start()) {
        std::cerr << "����������ʧ��!" << std::endl;
        std::cout << "����˿�8080�Ƿ�ռ��" << std::endl;
        return -1;
    }

    std::cout << "�����������ɹ�! ��Enter��ֹͣ������" << std::endl;
    std::cout << "�����������˿�: 8080" << std::endl;
    std::cout << "�ȴ��ͻ�������..." << std::endl;

    // ����һ���߳����з�����
    std::thread serverThread(&Server::run, &server);

    //server.run();

    // �ȴ��û�������ֹͣ������
    std::string input;
    std::getline(std::cin, input);

    std::cout << "����ֹͣ������..." << std::endl;
    server.stop();

    if (serverThread.joinable()) {
        serverThread.join();
    }

    std::cout << "��������ֹͣ" << std::endl;
    return 0;
}