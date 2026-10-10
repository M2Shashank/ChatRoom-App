#include "chatRoom.hpp"

// Initializes a session with an accepted socket and the room it belongs to.
Session::Session(tcp::socket s, Room &room)
    : clientSocket(std::move(s)), room(room) {}

// Adds a participant to the room's participant set.
void Room::join(ParticipantPointer participant) {
    this -> participants.insert(participant);
}

// Removes a participant from the room's participant set.
void Room::leave(ParticipantPointer participant) {
    this -> participants.erase(participant);
}

// Broadcasts a message to all participants other than the sender.
void Room::deliver(ParticipantPointer participant, Message &message) {
    messageQueue.push_back(message);
    while(!messageQueue.empty()) {
        Message msg = messageQueue.front();
        messageQueue.pop_front();

        for(ParticipantPointer _participant: participants) {
            if(_participant != participant) {
                _participant -> write(msg);
            }
        }
    }
}

// Starts an asynchronous read and processes each complete client message.
void Session::async_read() {
    auto self(shared_from_this());

    boost::asio::async_read_until(clientSocket, buffer, "\n", [this, self](boost::system::error_code ec, std::size_t bytes_transferred) {
        if(!ec) {
            std::string data(boost::asio::buffers_begin(buffer.data()), boost::asio::buffers_begin(buffer.data()) + bytes_transferred);

            buffer.consume(bytes_transferred);
            std::cout << "Recieved: " << data << std::endl;
            Message message(data);
            deliver(message);
            async_read();
        }else {
            room.leave(shared_from_this());
            if(ec == boost::asio::error::eof) {
                std::cout << "Connection closed by peer" << std::endl;
            }else {
                std::cout << "Read error: " << ec.message() << std::endl;
            }
        }
    });
}

// Starts an asynchronous write of a message body to the client.
void Session::async_write(std::string messageBody, size_t messageLength) {
    auto write_handler = [&](boost::system::error_code ec, std::size_t bytes_transferred) {
        if(!ec) {
            std::cout << "Data is written to the socket" << std::endl;
        }else {
            std::cerr << "Write error: " << ec.message() << std::endl;
        }
    };
    boost::asio::async_write(clientSocket, boost::asio::buffer(messageBody, messageLength), write_handler);
}

// Decodes, queues, and sends a message to this session's client.
void Session::write(Message &message) {
    messageQueue.push_back(message);
    while(messageQueue.size() != 0) {
        Message message = messageQueue.front();
        messageQueue.pop_front();

        bool header_decode = message.decodeHeader();
        if(header_decode) {
            std::string body = message.getBody();
            async_write(body, message.getBodyLength());
        }else {
            std::cout << "Message length exceeds the max length" << std::endl;
        }
    }
}

// Sends a message from this session to the room for broadcasting.
void Session::deliver(Message &message) {
    room.deliver(shared_from_this(), message);
}

// Registers the session with the room and begins reading from the client.
void Session::start() {
    room.join(shared_from_this());
    async_read();
}

using boost::asio::ip::address_v4;

// Accepts clients asynchronously and creates a session for each connection.
void accept_connection(boost::asio::io_context &io, char *port, tcp::acceptor &acceptor, Room &room, const tcp::endpoint &endpoint) {
    tcp::socket socket(io);
    acceptor.async_accept([&](boost::system::error_code ec, tcp::socket socket) {
        if(!ec) {
            std::shared_ptr<Session> session = std::make_shared<Session>(std::move(socket), room );
            session -> start();
        }
        accept_connection(io, port, acceptor, room, endpoint);
    });
}

// Creates the server, starts accepting connections, and runs the I/O loop.
int main(int argc, char *argv[]) {
    try {
        if(argc < 2) {
            std::cerr << "Usage: server <port>";
            return 1;
        }
        Room room;
        boost::asio::io_context io_context;
        tcp::endpoint endpoint(tcp::v4(), atoi(argv[1]));
        tcp::acceptor acceptor(io_context, endpoint);
        accept_connection(io_context, argv[1], acceptor, room, endpoint);
        io_context.run();
    }catch(std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    return 0;
}