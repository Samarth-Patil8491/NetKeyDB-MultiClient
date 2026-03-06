# NetKeyDB-MultiClient

**NetKeyDB – Multi-Client In-Memory Key-Value Database (C++)**
**Overview**

NetKeyDB is a lightweight in-memory key-value database server built in C++ using TCP socket programming.
It allows multiple clients to connect simultaneously and execute basic database commands over a network connection.

The project demonstrates core system programming concepts such as:

TCP networking

Multithreading

Concurrent data access

Command parsing

In-memory database design

This project is inspired by the basic architecture of systems like Redis, but simplified for learning purposes.

** Features**

✔ Multi-client TCP server
✔ Thread-safe in-memory database
✔ Concurrent client handling using std::thread
✔ Mutex-protected database operations
✔ Simple command-based protocol
✔ Lightweight and fast

**Technologies Used**

C++

WinSock API

STL (unordered_map)

Multithreading (std::thread)

Mutex (std::mutex)

Socket Programming

**How It Works**

The server starts and listens on TCP port 8080.

Clients connect using tools like Telnet or Netcat.

Each client connection is handled in a separate thread.

Commands are parsed and executed on a shared in-memory database.

A mutex lock ensures thread-safe database operations.

**Supported Commands**
**SET**

Stores a key-value pair.

SET key value

**Example**

SET name Alice

**Response**

OK
**GET**

Retrieves the value of a key.

GET key

**Example**

GET name

**Response**

Alice

If key does not exist:

NULL
**DEL**

Deletes a key from the database.

DEL key

**Example**

DEL name

**Response**

1

If key does not exist

0
**EXIT**

Disconnects the client.

EXIT

**Installation & Build**
**Requirements**

MinGW / MSYS2

g++ compiler

Windows OS

**Compile**
g++ server.cpp -o server -lws2_32

**Run**
server

**Server output**

Server started on port 8080...

**Connecting to the Server**

**Using Netcat (ncat)**

**ncat localhost 8080**

**Or using Telnet
telnet localhost 8080**

**Example Session**
SET name John
OK

GET name
John

DEL name
1

GET name
NULL


**Project Structure**
NetKeyDB
│
├── server.cpp
└── README.md

**Learning Outcomes**

This project helps understand:

TCP socket programming

Concurrent server design

Thread safety using mutex

Network protocol design

Building basic database systems


**Future Improvements**

Possible upgrades for the project:

Authentication system

Persistent database storage

Command logging

Admin commands

SSL/TLS encryption

Redis-style protocol

Web dashboard

**License**

This project is open-source and available for educational purposes.
