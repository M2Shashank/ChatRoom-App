#include <iostream>
#include "message.hpp"
#include <set>
#include <memory>
#include <sys/socket.h>
#include <boost/asio.hpp>
#include <deque>


#ifndef CHATROOM_HPP
#define CHATROOM_HPP

using boost::asio::ip::tcp;

class Participant {
    public:
        // Delivers a message through the participant's chat connection.
        virtual void deliver(Message &message) = 0;
        // Writes a message to the participant's chat connection.
        virtual void write(Message &message) = 0;
        // Allows derived participant objects to be destroyed safely.
        virtual ~Participant() = default;
};

typedef std::shared_ptr<Participant> ParticipantPointer;

class Room {
    public:
        // Adds a participant to the room.
        void join(ParticipantPointer participant);
        // Removes a participant from the room.
        void leave(ParticipantPointer participant);
        // Broadcasts a message to every room participant except the sender.
        void deliver(ParticipantPointer participant, Message &messsage);
    private:
        std::deque<Message> messageQueue;
        enum {MaxParticipants = 100};
        std::set<ParticipantPointer> participants;
};

class Session: public Participant, public std::enable_shared_from_this<Session> {
    public:
        // Creates a session using an accepted socket and its containing room.
        Session(tcp::socket s, Room &room);
        // Joins the room and starts reading messages from the client.
        void start();
        // Passes a message from this session to the room for broadcasting.
        void deliver(Message &message);
        // Queues and sends a message to this session's client.
        void write(Message &message);
        // Asynchronously reads a newline-terminated message from the client.
        void async_read();
        // Asynchronously writes a message body to the client socket.
        void async_write(std::string messageBody, size_t messageLength);
    private:
        tcp::socket clientSocket;
        boost::asio::streambuf buffer;
        Room &room;
        std::deque<Message> messageQueue;
};


#endif