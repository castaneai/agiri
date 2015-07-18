#include "precompiled_header.hpp"
#include "global.hpp"
#include "RequestHandler.hpp"
#include "NinjaConnection.hpp"
#include "SocketHelper.hpp"

RequestHandler::RequestHandler(const NinjaConnection& conn)
    : m_ninjaConnection(conn)
{}

void RequestHandler::handle(const Message& request) const
{
    switch (request.command)
    {
    case Command::PingRequest:
        respondPong();
        break;

    case Command::ListSocketRequest:
        respondSocketsList();
        break;

    case Command::InjectOutgoingPacketRequest:
        injectOutgoingPacket(reinterpret_cast<const InjectOutgoingPacketRequest&>(request.data));
        break;

    case Command::SwitchSniffIncomingPacketRequest:
        switchSniffIncomingPacket(reinterpret_cast<const SwitchSniffIncomingPacketRequest&>(request.data));
        break;

    default:
        // TODO: log error
        break;
    }
}

void RequestHandler::respondPong() const
{
    m_ninjaConnection.send(Command::PongResponse, nullptr, 0);
}

void RequestHandler::respondSocketsList() const
{
    ListSocketResponse response;
    response.socketsCount = SocketHelper::getAllSockets(response.sockets);
    m_ninjaConnection.send(Command::ListSocketResponse, reinterpret_cast<byte_t*>(&response), response.getSize());
}

void RequestHandler::injectOutgoingPacket(const InjectOutgoingPacketRequest& request) const
{
    global::original_api::send(request.targetSocket, reinterpret_cast<char*>(const_cast<byte_t*>(request.packetData)), request.packetSize, 0);
}

void RequestHandler::switchSniffIncomingPacket(const SwitchSniffIncomingPacketRequest& request) const
{
    if (request.state) {
        global::incomingPacketSniffers[request.targetSocket] = this;
    }
    else {
        global::incomingPacketSniffers.erase(request.targetSocket);
    }
}

// TODO: refactor
void RequestHandler::respondSniffIncomingPacket(const socket_t& sock, const byte_t* const packet, const uint32_t& packetSize) const
{
    auto command = Command::IncomingPacketResponse;
    SniffIncomingPacketResponseHeader responseHeader;
    responseHeader.socket = sock;
    responseHeader.packetSize = packetSize;
    const uint32_t messageSize = sizeof(responseHeader) + packetSize;
    m_ninjaConnection.sendChunk(reinterpret_cast<const byte_t* const>(&command), sizeof(command));
    m_ninjaConnection.sendChunk(reinterpret_cast<const byte_t* const>(&messageSize), sizeof(messageSize));
    m_ninjaConnection.sendChunk(reinterpret_cast<const byte_t* const>(&responseHeader), sizeof(responseHeader));
    m_ninjaConnection.sendChunk(packet, packetSize);
}