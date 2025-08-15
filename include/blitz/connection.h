#pragma once
#include <atomic>
#include <string>


namespace blitz {
    class Address {
        std::string host_;
        short port_;

        bool operator==(const Address& other) const {
            return host_ == other.host_ && port_ == other.port_;
        }
    public:
        Address(const std::string &host, const short port) : host_(host), port_(port) {}
        const std::string& host() const { return host_; }
        short port() const { return port_; }
    };

    class Read {
    public:
        Read(instance_t instance, socket_t fd, size_t buffer_size);
    private:
        socket_t fd;
        size_t buffer_size;
    };

    class Write {
    public:
        Write(instance_t instance, socket_t fd, size_t buffer_size);
    private:
        socket_t fd;
        size_t buffer_size;
    };

    class Connection {
    public:
        Connection(instance_t instance, socket_t fd, size_t buffer_size, Address address);
        Address& addr();
        Read& read();
        Write& write();
        bool close();
    private:
        Address address;
        socket_t fd;
        size_t buffer_size;
        std::atomic_bool closed;
        Read reader;
        Write writer;
    };
}

namespace std {
    template<>
    struct hash<blitz::Address> {
        std::size_t operator()(const blitz::Address& addr) const noexcept {
            std::size_t h1 = std::hash<std::string>{}(addr.host());
            std::size_t h2 = std::hash<short>{}(addr.port());
            return h1 ^ (h2 << 1); // combine hashes
        }
    };
}

