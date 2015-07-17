#pragma once
#include "messages.hpp"
class NinjaConnection;

class RequestHandler final
{
public:
    RequestHandler(const NinjaConnection&);
    void handle(const Message&) const;
private:
    void respondPong() const;
    void respondSocketsList() const;
    void injectOutgoingPacket(const InjectOutgoingPacketRequest&) const;
    const NinjaConnection& m_ninjaConnection;
};