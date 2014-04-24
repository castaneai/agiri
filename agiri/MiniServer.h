#pragma once

namespace agiri {
    namespace server {
        class MiniServer
        {
        public:
            MiniServer(const char* ipAddress, const unsigned short port);
            void receiveForever() const;

        private:
            const char* ipAddress;
            const unsigned short port;
        };
    }
}