# ChatRoom

ChatRoom is a small C++ chat application built with Boost.Asio. It provides a simple TCP chat server that accepts multiple client connections and broadcasts incoming messages to other participants in the same room.

## Features

- Asynchronous TCP server using Boost.Asio
- Multi-client room support
- Message broadcasting to all other connected clients
- Length-prefixed message protocol with a fixed maximum payload size
- Simple command-line client for connecting to the server

## Project Structure

- `chatRoom.cpp` – server entry point and connection acceptance loop
- `chatRoom.hpp` – room, participant, and session abstractions
- `client.cpp` – example client application
- `message.hpp` – message framing and encoding logic
- `Makefile` – build targets for the server and client

## Requirements

- C++20 compatible compiler (`g++`)
- Boost.Asio development libraries
- pthread support

On Debian/Ubuntu-based systems, you can install the required libraries with:

```bash
sudo apt-get update
sudo apt-get install libboost-system-dev libboost-thread-dev
```

## Build

From the project root, run:

```bash
make
```

This produces:

- `chatApp` – the chat server
- `clientApp` – the chat client

## Run the Server

Start the chat server on a chosen port:

```bash
./chatApp 8080
```

## Run the Client

In another terminal window, start a client and connect to the same port:

```bash
./clientApp 8080
```

You can launch multiple client instances to simulate a room. Type a message in one client window and it will be sent to the server, which then broadcasts it to the other connected clients.

## Notes

- The server expects a port argument when launched.
- Messages are terminated by a newline (`\n`) in the client input flow.
