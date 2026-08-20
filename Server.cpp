// NetKey Database (DB) - Multi-Client Server

#include <iostream>
#include <unordered_map>
#include <sstream>
#include <winsock2.h>
#include <algorithm>
#include <thread>
#include <mutex>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

// Shared key-value database
unordered_map<string, string> database;

// Mutex to protect the shared database
mutex db_mutex;

// User database: username -> password
unordered_map<string, string> users = {
    {"admin", "1234"},
    {"user1", "pass1"},
    {"user2", "pass2"}
};


// Handles communication with one client
void handleClient(SOCKET client_socket) {

    char buffer[1024];

    // Authentication status for this client
    bool authenticated = false;
    string currentUser = "";

    while (true) {

        memset(buffer, 0, sizeof(buffer));

        // Receive command from client
        int bytes = recv(
            client_socket,
            buffer,
            sizeof(buffer) - 1,
            0
        );

        // Client disconnected or error occurred
        if (bytes <= 0) {
            break;
        }

        buffer[bytes] = '\0';

        string line(buffer);

        // Remove newline characters
        line.erase(
            remove(line.begin(), line.end(), '\r'),
            line.end()
        );

        line.erase(
            remove(line.begin(), line.end(), '\n'),
            line.end()
        );

        if (line.empty()) {
            continue;
        }

        // Parse command
        stringstream ss(line);

        string command;
        string key;
        string value;
        string extra;

        ss >> command;

        string response;


        // ==========================================
        // AUTH COMMAND
        // ==========================================

        if (command == "AUTH" || command == "auth") {

            string user;
            string pass;

            ss >> user >> pass >> extra;

            // Check command format
            if (user.empty() || pass.empty() || !extra.empty()) {

                response =
                    "Error: Usage -> AUTH username password\n";
            }

            // Check username and password
            else if (
                users.find(user) != users.end() &&
                users[user] == pass
            ) {

                authenticated = true;
                currentUser = user;

                response =
                    "AUTH OK. Welcome " + user + "\n";
            }

            else {

                response = "AUTH FAILED\n";
            }
        }


        // ==========================================
        // BLOCK COMMANDS IF NOT AUTHENTICATED
        // ==========================================

        else if (!authenticated) {

            response =
                "Please login first using AUTH username password\n";
        }


        // ==========================================
        // SET COMMAND
        // ==========================================

        else if (command == "SET" || command == "set") {

            ss >> key >> value >> extra;

            // Validate command
            if (
                key.empty() ||
                value.empty() ||
                !extra.empty()
            ) {

                response =
                    "Error: Usage -> SET key value\n";
            }

            else {

                // Lock database before modifying it
                lock_guard<mutex> lock(db_mutex);

                database[key] = value;

                response =
                    "OK (set by " + currentUser + ")\n";
            }
        }


        // ==========================================
        // GET COMMAND
        // ==========================================

        else if (command == "GET" || command == "get") {

            ss >> key >> extra;

            if (key.empty() || !extra.empty()) {

                response =
                    "Error: Usage -> GET key\n";
            }

            else {

                // Lock database while accessing it
                lock_guard<mutex> lock(db_mutex);

                if (database.find(key) != database.end()) {

                    response =
                        database[key] + "\n";
                }

                else {

                    response = "NULL\n";
                }
            }
        }


        // ==========================================
        // DEL COMMAND
        // ==========================================

        else if (command == "DEL" || command == "del") {

            ss >> key >> extra;

            if (key.empty() || !extra.empty()) {

                response =
                    "Error: Usage -> DEL key\n";
            }

            else {

                // Lock database before deleting
                lock_guard<mutex> lock(db_mutex);

                if (database.erase(key)) {

                    response = "1\n";
                }

                else {

                    response = "0\n";
                }
            }
        }


        // ==========================================
        // EXIT COMMAND
        // ==========================================

        else if (command == "EXIT" || command == "exit") {

            response =
                "Disconnected from server\n";

            send(
                client_socket,
                response.c_str(),
                response.size(),
                0
            );

            break;
        }


        // ==========================================
        // UNKNOWN COMMAND
        // ==========================================

        else {

            response = "Unknown command\n";
        }


        // Send response to client
        send(
            client_socket,
            response.c_str(),
            response.size(),
            0
        );
    }


    // Close client connection
    closesocket(client_socket);

    cout << "Client disconnected\n";
}


// ==========================================
// MAIN SERVER
// ==========================================

int main() {

    // Initialize Winsock
    WSADATA wsa;

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {

        cout << "WSAStartup failed!\n";

        return 1;
    }


    // Create TCP socket
    SOCKET server_fd = socket(
        AF_INET,
        SOCK_STREAM,
        0
    );

    if (server_fd == INVALID_SOCKET) {

        cout << "Socket creation failed!\n";

        WSACleanup();

        return 1;
    }


    // Configure server address
    sockaddr_in address;

    address.sin_family = AF_INET;

    address.sin_addr.s_addr = INADDR_ANY;

    address.sin_port = htons(8080);


    // Bind socket to port 8080
    if (
        bind(
            server_fd,
            (sockaddr*)&address,
            sizeof(address)
        ) == SOCKET_ERROR
    ) {

        cout << "Bind failed!\n";

        closesocket(server_fd);

        WSACleanup();

        return 1;
    }


    // Start listening for clients
    if (listen(server_fd, 5) == SOCKET_ERROR) {

        cout << "Listen failed!\n";

        closesocket(server_fd);

        WSACleanup();

        return 1;
    }


    cout << "====================================\n";
    cout << "      NetKey Multi-Client Server\n";
    cout << "====================================\n";
    cout << "Server started on port 8080...\n";
    cout << "Waiting for clients...\n";


    // Accept clients continuously
    while (true) {

        SOCKET client_socket =
            accept(
                server_fd,
                nullptr,
                nullptr
            );

        if (client_socket == INVALID_SOCKET) {

            cout << "Failed to accept client.\n";

            continue;
        }


        cout << "Client connected!\n";


        // Create a separate thread for the client
        thread clientThread(
            handleClient,
            client_socket
        );


        // Allow the thread to run independently
        clientThread.detach();
    }


    // Close server
    closesocket(server_fd);

    WSACleanup();

    return 0;
}