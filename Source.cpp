#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

void displayLocalIPAddress() {
    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) == SOCKET_ERROR) {
        std::cerr << "Error getting hostname. Error: " << WSAGetLastError() << '\n';
        return;
    }

    struct addrinfo hints = {}, * info;
    hints.ai_family = AF_INET;  // Only want IPv4 addresses
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    if (getaddrinfo(hostname, NULL, &hints, &info) != 0) {
        std::cerr << "Error getting address info. Error: " << WSAGetLastError() << '\n';
        return;
    }

    for (struct addrinfo* p = info; p != NULL; p = p->ai_next) {
        struct sockaddr_in* sockaddr_ipv4 = (struct sockaddr_in*)p->ai_addr;
        char ipAddress[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(sockaddr_ipv4->sin_addr), ipAddress, INET_ADDRSTRLEN);
        std::cout << "Local IP Address: " << ipAddress << '\n';
    }

    freeaddrinfo(info);
}

void openPort(int port) {
    WSADATA wsaData;
    SOCKET listenSocket = INVALID_SOCKET, clientSocket = INVALID_SOCKET;
    struct sockaddr_in serverAddr = {};

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed.\n";
        return;
    }

    // Display the local IP address
    displayLocalIPAddress();

    // Create a socket for listening
    listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed.\n";
        WSACleanup();
        return;
    }

    // Bind the socket to the specified port
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(listenSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed. Error: " << WSAGetLastError() << '\n';
        closesocket(listenSocket);
        WSACleanup();
        return;
    }

    // Listen for a connection
    if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Listen failed. Error: " << WSAGetLastError() << '\n';
        closesocket(listenSocket);
        WSACleanup();
        return;
    }

    std::cout << "Waiting for a connection on port " << port << "...\n";

    // Accept a client socket
    clientSocket = accept(listenSocket, NULL, NULL);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Accept failed. Error: " << WSAGetLastError() << '\n';
    }
    else {
        std::cout << "Success! Port " << port << " is open and a connection was made.\n";
    }

    // Clean up and close the sockets
    std::cout << "Press Enter to close the port and try a new one.\n";
    std::cin.get();  // Pause until user presses Enter
    closesocket(clientSocket);
    closesocket(listenSocket);
    WSACleanup();
}

int main() {
    int port;

    while (true) {
        std::cout << "Enter port to open (or 0 to exit): ";
        std::cin >> port;
        std::cin.ignore();  // Ignore newline character from std::cin
        if (port == 0) break;  // Exit if port is 0

        openPort(port);
    }

    std::cout << "Exiting program.\n";
    return 0;
}
