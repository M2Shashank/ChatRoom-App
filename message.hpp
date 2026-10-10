#include <iostream>
#include <string>
#include <cstring>


#ifndef MESSAGE_HPP
#define MESSAGE_HPP


class Message {
    public:
        enum {maxBytes = 512};
        enum {header = 4};
        // Creates an empty message with no body.
        Message(): bodyLength_(0) {}

        // Creates a message, stores its body, and encodes its length header.
        Message(std::string message) {
            bodyLength_ = getNewBodyLength(message.length());
            encodeHeader();
            std::memcpy(data + header, message.c_str(), bodyLength_);
        }

        // Limits a requested body length to the maximum supported size.
        size_t getNewBodyLength(size_t newLength) {
            if(newLength > maxBytes) {
                return maxBytes;
            }
            return newLength;
        }

        // Returns the encoded header followed by the message body.
        std::string getData() {
            int length = header + bodyLength_;
            std::string result(data, length);
            return result;
        }

        // Returns only the message body without its length header.
        std::string getBody() {
            std::string dataStr = getData();
            std::string result = dataStr.substr(header, bodyLength_);
            return result;
        }

        // Reads the body length from the encoded header.
        bool decodeHeader() {
            char new_header[header+1] = "";
            strncpy(new_header, data, header);
            new_header[header] = '\0';
            int headerValue = atoi(new_header);
            if(headerValue > maxBytes) {
                bodyLength_ = 0;
                return false;
            }
            bodyLength_ = headerValue;
            return true;
        }

        // Writes the current body length into the message header.
        void encodeHeader() {
            char new_header[header + 1] = "";
            sprintf(new_header, "%04d", static_cast<int>(bodyLength_));
            memcpy(data, new_header, header);
        }

        // Returns the length of the message body.
        size_t getBodyLength() {
            return bodyLength_;
        }

    private:
        char data[header + maxBytes];
        size_t bodyLength_;
};


#endif