#pragma once
#include <atomic>
#include <string>


namespace blitz {
    class address {
        std::string host_;
        short port_;

        bool operator==(const address& other) const {
            return host_ == other.host_ && port_ == other.port_;
        }
    public:
        address(const std::string &host, const short port) : host_(host), port_(port) {}
        const std::string& host() const { return host_; }
        short port() const { return port_; }
    };

    class reader {
    public:
        reader(socket_t fd, size_t buffer_size);
    private:
        socket_t fd;
        size_t buffer_size;
    };

    class writer {
    public:
        writer(socket_t fd, size_t buffer_size);
    private:
        socket_t fd;
        size_t buffer_size;
    };

    class connection {
    public:
        connection(socket_t fd, size_t buffer_size);
        reader& read();
        writer& write();
        void close();
    private:
        socket_t fd;
        size_t buffer_size;
        std::atomic_bool closed;
        reader reader_;
        writer writer_;
    };
}

namespace std {
    template<>
    struct hash<blitz::address> {
        std::size_t operator()(const blitz::address& addr) const noexcept {
            std::size_t h1 = std::hash<std::string>{}(addr.host());
            std::size_t h2 = std::hash<short>{}(addr.port());
            return h1 ^ (h2 << 1); // combine hashes
        }
    };
}

