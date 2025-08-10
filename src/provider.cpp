#include "blitz/provider.h"

#ifdef PLATFORM_WINDOWS
#include <windows.h>
#include <winsock2.h>
#elif defined(PLATFORM_LINUX)
#include <sys/epoll.h>
#include <unistd.h>
#endif

#include <iostream>

namespace blitz {
    provider::provider(const size_t worker_threads, const size_t buffer_size) : servers(), buffer_size(buffer_size) {
        #ifdef PLATFORM_WINDOWS
            std::cout << "Runnign on windows" << std::endl;
        #elif defined(PLATFORM_LINUX)
            std::cout << "Runnign on linux" << std::endl;
        #endif
    }

    provider::~provider() {
        close();
    }

    void provider::accept(const address& server_address, const std::function<void(std::unique_ptr<connection>)>& callback) {
        #ifdef PLATFORM_WINDOWS
            // here
        #elif defined(PLATFORM_LINUX)
            // here
        #endif
    }

    void provider::connect(const address& server_address, const std::function<void(std::unique_ptr<connection>)>& callback) {
        #ifdef PLATFORM_WINDOWS
            // here
        #elif defined(PLATFORM_LINUX)
            // here
        #endif
    }

    bool provider::close() {
        if (closed.exchange(true)) {
            return false;
        }
        #ifdef PLATFORM_WINDOWS
            // here
        #elif defined(PLATFORM_LINUX)
            // here
        #endif
        return true;
    }

}