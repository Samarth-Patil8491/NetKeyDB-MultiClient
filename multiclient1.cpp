#include <iostream>
#include <unordered_map>
#include <sstream>
#include <winsock2.h>
#include <algorithm>
#include <thread>
#include <mutex>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

// 🌍 Shared database
unordered_map<string, string> database;
mutex db_mutex;   // protects database

// 👤 Client handler function
void handleClient(SOCKET client_socket) {
    char buffer[1024];

    while (true) {
        memset(buffer, 0, sizeof(buffer));

        int bytes = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (bytes <= 0) break;

        buffer[bytes] = '\0';
        string line(buffer);

        // Clean newline characters
        line.erase(remove(line.begin(), line.end(), '\r'), line.end());
        line.erase(remove(line.begin(), line.end(), '\n'), line.end());

        if (line.empty()) continue;

        stringstream ss(line);
        string command, key, value, extra;
        ss >> command;

        string response;

        if (command == "SET" || command == "set") {
            ss >> key >> value >> extra;

            if (key.empty() || value.empty() || !extra.empty()) {
                response = "Error: Usage -> SET key value\n";
            } else {
                lock_guard<mutex> lock(db_mutex);   // 🔒 thread safety
                database[key] = value;
                response = "OK\n";
            }
        }
        else if (command == "GET" || command == "get") {
            ss >> key;

            lock_guard<mutex> lock(db_mutex);
            if (database.find(key) != database.end())
                response = database[key] + "\n";
            else
                response = "NULL\n";
        }
        else if (command == "DEL" || command == "del") {
            ss >> key;

            lock_guard<mutex> lock(db_mutex);
            response = database.erase(key) ? "1\n" : "0\n";
        }
        else if (command == "EXIT" || command == "exit") {
            response = "Disconnected from server\n";
            send(client_socket, response.c_str(), response.size(), 0);
            break;
        }
        else {
            response = "Unknown command\n";
        }

        send(client_socket, response.c_str(), response.size(), 0);
    }

    closesocket(client_socket);
    cout << "Client disconnected\n";
}

int main() {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    SOCKET server_fd = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    bind(server_fd, (sockaddr*)&address, sizeof(address));
    listen(server_fd, 5);

    cout << "🚀 Server started on port 8080...\n";

    // 🔁 Accept clients forever
    while (true) {
        SOCKET client_socket = accept(server_fd, nullptr, nullptr);
        cout << "Client connected!\n";

        thread t(handleClient, client_socket);
        t.detach();   // run independently
    }

    closesocket(server_fd);
    WSACleanup();
    return 0;
}