# NetKeyDB - Multi-Client TCP Key-Value Database

NetKeyDB is a simple multi-client key-value database developed in C++ to explore TCP networking, client-server communication, authentication, and concurrent client handling.

## Features

- TCP client-server communication using Windows Winsock
- Multiple clients connected to a single server
- Separate thread for each connected client
- Shared in-memory key-value database
- Mutex protection for shared database access
- Basic username/password authentication
- Command-based interaction
- Input validation and basic error handling

## Commands

### AUTH

Authenticate a client.

```text
AUTH username password