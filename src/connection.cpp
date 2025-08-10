#include "blitz/temp.h"
#include "blitz/connection.h"

#ifdef PLATFORM_WINDOWS
  #include <winsock2.h>
#elif defined(PLATFORM_LINUX)
  #include <unistd.h>
#endif

namespace blitz {
    reader::reader(const socket_t fd, const size_t buffer_size) : fd(fd), buffer_size(buffer_size) {

    }

    writer::writer(const socket_t fd, const size_t buffer_size) : fd(fd), buffer_size(buffer_size) {

    }

    connection::connection(const socket_t fd, const size_t buffer_size):
        fd(fd),
        buffer_size(buffer_size),
        closed(false),
        reader_(fd, buffer_size),
        writer_(fd, buffer_size) {}

    reader& connection::read() { return reader_; }
    writer& connection::write() { return writer_; }

    void connection::close() {
        if (closed.exchange(true)) return;

        #ifdef PLATFORM_WINDOWS
            // closesocket(fd);
        #elif defined(PLATFORM_LINUX)
            // ::close(fd);
        #endif
    }
}