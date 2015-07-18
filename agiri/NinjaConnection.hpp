/**
 * NinjaConnection: a connection between agiri.dll and external sniffer.
 * This connection uses struct Message as protocol.
 * Please see "messages.hpp" for details.
 *
 * agiri.dllとsniffer（外部プログラム）との通信を担当する
 * 通信はすべてMessage構造体のやりとりで実行される
 * 具体的な構造は messages.hpp を参照のこと．
 */
#pragma once
#include "types.hpp"
#include "messages.hpp"

class NinjaConnection final
{
    NO_DEFAULT_CONSTRUCTOR(NinjaConnection)
public:
    NinjaConnection(const socket_t&);
    virtual ~NinjaConnection();
    void send(const Command&, const byte_t* const, const uint32_t& dataSize) const;
    void send(const Message&) const;
    void sendChunk(const byte_t* const chunk, const uint32_t& chunkSize) const;
    void receive(Message&) const;
private:
    const socket_t& m_sock;
    inline bool sendAll(const byte_t* const, const uint32_t&) const;
    inline bool recvAll(byte_t* const, const uint32_t&) const;
};