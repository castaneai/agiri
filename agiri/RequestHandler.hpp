#pragma once
#include "messages.hpp"
class NinjaConnection;

class RequestHandler final
{
public:
    RequestHandler(const NinjaConnection& conn);
    void handle(const Message& message) const;
    void respondSniffIncomingPacket(const socket_t& sock, const byte_t* const packet, const uint32_t& packetSize) const;
private:
    const NinjaConnection& m_ninjaConnection;
    void respondPong() const;
    void respondSocketsList() const;
    void injectOutgoingPacket(const InjectOutgoingPacketRequest& request) const;
    void switchSniffIncomingPacket(const SwitchSniffIncomingPacketRequest& request) const;
};