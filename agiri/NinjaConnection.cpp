#include "precompiled_header.hpp"
#include "NinjaConnection.hpp"
#include "messages.hpp"
#include "global.hpp"

NinjaConnection::NinjaConnection(const socket_t& sock)
    : m_sock(sock)
{
}

NinjaConnection::~NinjaConnection()
{
    closesocket(m_sock);
}

bool NinjaConnection::send(const Command& command, const byte_t* const data, const uint32_t& dataSize) const
{
    if (!sendAll(reinterpret_cast<const byte_t*>(&command), sizeof(command))) {
        return false;
    }
    if (!sendAll(reinterpret_cast<const byte_t*>(&dataSize), sizeof(dataSize))) {
        return false;
    }
    if (!sendAll(data, dataSize)) {
        return false;
    }
    return true;
}

bool NinjaConnection::send(const Message& message) const
{
    if (!sendAll(reinterpret_cast<const byte_t*>(&message), message.getSize())) {
        return false;
    }
    return true;
}

bool NinjaConnection::sendChunk(const byte_t* const chunk, const uint32_t& chunkSize) const
{
    if (!sendAll(chunk, chunkSize)) {
        return false;
    }
    return true;
}

bool NinjaConnection::receive(Message& message) const
{
    if (!recvAll(reinterpret_cast<byte_t*>(&message.command), sizeof(message.command))) {
        return false;
    }
    if (!recvAll(reinterpret_cast<byte_t*>(&message.dataSize), sizeof(message.dataSize))) {
        return false;
    }
    if (!recvAll(message.data, message.dataSize)) {
        return false;
    }
    return true;
}

inline bool NinjaConnection::sendAll(const byte_t* const input, const uint32_t& size) const
{
    uint32_t sent = 0;
    while (sent < size) {
        int result = global::original_api::send(m_sock, reinterpret_cast<char*>(const_cast<byte_t*>(input)) + sent, size - sent, 0);
        if (result < 1) return false;
        sent += result;
    }
    return true;
}

inline bool NinjaConnection::recvAll(byte_t* const output, const uint32_t& size) const
{
    uint32_t received = 0;
    while (received < size) {
        int result = global::original_api::recv(m_sock, reinterpret_cast<char*>(output) + received, size - received, 0);
        if (result < 1) return false;
        received += result;
    }
    return true;
}
