#include "blitz/temp.h"
#include "blitz/connection.h"

#include <stdexcept>

#ifdef PLATFORM_WINDOWS
  #include <winsock2.h>

#include <utility>
#elif defined(PLATFORM_LINUX)
  #include <unistd.h>
#endif

namespace blitz {
    Read::Read(const instance_t instance, socket_t fd, const size_t buffer_size) : fd(fd), buffer_size(buffer_size) {
        #ifdef PLATFORM_WINDOWS

        #elif defined(PLATFORM_LINUX)

        #endif
    }

    Write::Write(const instance_t instance, const socket_t fd, const size_t buffer_size) : fd(fd), buffer_size(buffer_size) {
        #ifdef PLATFORM_WINDOWS

        #elif defined(PLATFORM_LINUX)

        #endif
    }

    Connection::Connection(const instance_t instance, const socket_t fd, const size_t buffer_size, Address  address):
        address(std::move(address)),
        fd(fd),
        buffer_size(buffer_size),
        closed(false),
        reader(instance, fd, buffer_size),
        writer(instance, fd, buffer_size) {}

    Read& Connection::read() { return reader; }
    Write& Connection::write() { return writer; }
    Address& Connection::addr() { return address; }

    bool Connection::close() {
        if (closed.exchange(true)) return false;
        #ifdef PLATFORM_WINDOWS
            if (const auto result = closesocket(fd); result == SOCKET_ERROR) {
                throw std::runtime_error("Failed to close socket: " + WSAGetLastError());
            }
        #elif defined(PLATFORM_LINUX)
            if (const auto result = ::close(fd); result < 0) {
                throw std::runtime_error("Failed to close socket: " + strerror(errno));
            }
        #endif
        return true;
    }
}