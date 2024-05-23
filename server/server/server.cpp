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
    ADDRINFO hints;
    ADDRINFO* addrResult = nullptr;
    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ConnectSocket = INVALID_SOCKET;
    char recvBuffer[512];
    const char* sendBuffer = "Привет от сервера";

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
    hints.ai_flags = AI_PASSIVE;

    // Получение адресной информации для привязки сокета
    result = getaddrinfo(NULL, "666", &hints, &addrResult);
    if (result != 0) {
        cout << "getaddrinfo не удалось с ошибкой: " << result << endl;
        if (addrResult) freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    // Создание сокета
    ListenSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        cout << "Создание сокета не удалось" << endl;
        if (addrResult) freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    // Привязка сокета к адресу и порту
    result = bind(ListenSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen);
    if (result == SOCKET_ERROR) {
        cout << "Привязка не удалась, ошибка: " << result << endl;
        closesocket(ListenSocket);
        if (addrResult) freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    // Прослушивание входящих подключений
    result = listen(ListenSocket, SOMAXCONN);
    if (result == SOCKET_ERROR) {
        cout << "Прослушивание не удалось, ошибка: " << result << endl;
        closesocket(ListenSocket);
        if (addrResult) freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    // Принятие подключения от клиента
    ConnectSocket = accept(ListenSocket, NULL, NULL);
    if (ConnectSocket == INVALID_SOCKET) {
        cout << "Принятие соединения не удалось, ошибка: " << WSAGetLastError() << endl;
        closesocket(ListenSocket);
        if (addrResult) freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    // Закрытие слушающего сокета, он больше не нужен
    closesocket(ListenSocket);

    // Обработка сообщений от клиента
    do {
        ZeroMemory(recvBuffer, 512);
        result = recv(ConnectSocket, recvBuffer, 512, 0);
        if (result > 0) {
            cout << "Получено " << result << " байт" << endl;
            cout << "Полученные данные: " << recvBuffer << endl;

            // Отправка сообщения клиенту
            result = send(ConnectSocket, sendBuffer, (int)strlen(sendBuffer), 0);
            if (result == SOCKET_ERROR) {
                cout << "Отправка не удалась, ошибка: " << result << endl;
                closesocket(ConnectSocket);
                if (addrResult) freeaddrinfo(addrResult);
                WSACleanup();
                return 1;
            }
        }
        else if (result == 0) {
            cout << "Закрытие соединения" << endl;
        }
        else {
            cout << "Прием не удался, ошибка: " << WSAGetLastError() << endl;
            closesocket(ConnectSocket);
            if (addrResult) freeaddrinfo(addrResult);
            WSACleanup();
            return 1;
        }
    } while (result > 0);

    // Отключение отправки
    result = shutdown(ConnectSocket, SD_SEND);
    if (result == SOCKET_ERROR) {
        cout << "Отключение не удалось, ошибка: " << result << endl;
        closesocket(ConnectSocket);
        if (addrResult) freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    // Закрытие соединительного сокета
    closesocket(ConnectSocket);
    if (addrResult) freeaddrinfo(addrResult);
    WSACleanup();
    return 0;
}
