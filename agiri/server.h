#pragma once
#include <functional>

using ErrorCallbackFunc = std::function<void(const char* message, const int errorCode)>;

void startServer(const char* ipAddress, const unsigned short port, ErrorCallbackFunc onError);