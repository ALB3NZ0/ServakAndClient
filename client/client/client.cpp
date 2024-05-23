#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>

// Линковка с библиотекой WS2_32.lib
#pragma comment(lib, "Ws2_32.lib")

using namespace std;

int main() {

    setlocale(LC_ALL, "Russian");
    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    ADDRINFO hints;
    ADDRINFO* addrResult = nullptr;
    const char* sendBuffer1 = "Привет от клиента";
    const char* sendBuffer2 = "Второе сообщение от клиента";
    char recvBuffer[512];

    // Инициализация Winsock
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        cout << "WSAStartup не удалось с результатом: " << result << endl;
        return 1;
    }

    // Заполнение структуры hints
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Получение адресной информации для подключения к серверу
    result = getaddrinfo("127.0.0.1", "666", &hints, &addrResult);
    if (result != 0) {
        cout << "getaddrinfo не удалось с ошибкой: " << result << endl;
        if (addrResult) freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    // Создание сокета для подключения к серверу
    ConnectSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);
    if (ConnectSocket == INVALID_SOCKET) {
        cout << "Создание сокета не удалось" << endl;
        if (addrResult) freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    // Подключение к серверу
    result = connect(ConnectSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen);
    if (result == SOCKET_ERROR) {
        cout << "Подключение не удалось, ошибка: " << WSAGetLastError() << endl;
        closesocket(ConnectSocket);
        if (addrResult) freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(addrResult);

    // Отправка первого сообщения серверу
    result = send(ConnectSocket, sendBuffer1, (int)strlen(sendBuffer1), 0);
    if (result == SOCKET_ERROR) {
        cout << "Отправка не удалась, ошибка: " << WSAGetLastError() << endl;
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    // Прием ответа от сервера на первое сообщение
    result = recv(ConnectSocket, recvBuffer, 512, 0);
    if (result > 0) {
        recvBuffer[result] = '\0';
        cout << "Получено " << result << " байт" << endl;
        cout << "Полученные данные: " << recvBuffer << endl;
    }
    else if (result == 0) {
        cout << "Соединение закрыто" << endl;
    }
    else {
        cout << "Прием не удался, ошибка: " << WSAGetLastError() << endl;
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    // Отправка второго сообщения серверу
    result = send(ConnectSocket, sendBuffer2, (int)strlen(sendBuffer2), 0);
    if (result == SOCKET_ERROR) {
        cout << "Отправка не удалась, ошибка: " << WSAGetLastError() << endl;
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    // Прием ответа от сервера на второе сообщение
    result = recv(ConnectSocket, recvBuffer, 512, 0);
    if (result > 0) {
        recvBuffer[result] = '\0';
        cout << "Получено " << result << " байт" << endl;
        cout << "Полученные данные: " << recvBuffer << endl;
    }
    else if (result == 0) {
        cout << "Соединение закрыто" << endl;
    }
    else {
        cout << "Прием не удался, ошибка: " << WSAGetLastError() << endl;
    }

    // Закрытие сокета и очистка Winsock
    closesocket(ConnectSocket);
    WSACleanup();

    return 0;
}
