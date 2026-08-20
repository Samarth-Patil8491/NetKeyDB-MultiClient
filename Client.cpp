// NetKey Database (DB) - Multi-Client Client

#include <iostream>
#include <string>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

int main() {

    // Initialize Winsock
    WSADATA wsa;

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        cout << "WSAStartup failed!\n";
        return 1;
    }

    // Create client socket
    SOCKET client_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (client_socket == INVALID_SOCKET) {
        cout << "Socket creation failed!\n";
        WSACleanup();
        return 1;
    }

    // Server address
    sockaddr_in server_address;

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_address.sin_port = htons(8080);

    // Connect to server
    if (connect(
        client_socket,
        (sockaddr*)&server_address,
        sizeof(server_address)
    ) == SOCKET_ERROR) {

        cout << "Connection failed!\n";

        closesocket(client_socket);
        WSACleanup();

        return 1;
    }

    cout << "Connected to NetKey Server!\n";
    cout << "Server: 127.0.0.1:8080\n";

    cout << "\nAvailable commands:\n";
    cout << "  AUTH username password\n";
    cout << "  SET key value\n";
    cout << "  GET key\n";
    cout << "  DEL key\n";
    cout << "  EXIT\n";
    cout << "\n";

    string command;

    while (true) {

        cout << "NetKey> ";
        getline(cin, command);

        if (command.empty()) {
            continue;
        }

        // Send command to server
        int sent = send(
            client_socket,
            command.c_str(),
            command.length(),
            0
        );

        if (sent == SOCKET_ERROR) {
            cout << "Failed to send command.\n";
            break;
        }

        // Receive server response
        char buffer[1024];

        memset(buffer, 0, sizeof(buffer));

        int bytes = recv(
            client_socket,
            buffer,
            sizeof(buffer) - 1,
            0
        );

        if (bytes <= 0) {
            cout << "Server disconnected.\n";
            break;
        }

        buffer[bytes] = '\0';

        cout << "Server: " << buffer;

        // EXIT command closes the client
        if (command == "EXIT" || command == "exit") {
            break;
        }
    }

    // Close client socket
    closesocket(client_socket);

    // Clean up Winsock
    WSACleanup();

    cout << "\nClient stopped.\n";

    return 0;
}