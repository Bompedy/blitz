#include "blitz/provider.h"

#ifdef PLATFORM_WINDOWS
#include <windows.h>
#include <winsock2.h>
#include <mswsock.h>
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Mswsock.lib")

#elif defined(PLATFORM_LINUX)
#include <sys/epoll.h>
#include <unistd.h>
#include <arpa/inet.h>
#endif

#include <cstring>
#include <format>
#include <iostream>

#ifdef PLATFORM_WINDOWS
enum class OperationType : uint8_t {
    OP_READ,
    OP_WRITE,
    OP_ACCEPT,
    OP_CONNECT
};

struct io_context {
    OVERLAPPED overlapped{};
    OperationType type;

    explicit io_context(const OperationType op_type) : type(op_type) {
        std::memset(&overlapped, 0, sizeof(OVERLAPPED));
    }
};

struct accept_context : public io_context {
    SOCKET accept_socket = INVALID_SOCKET;
    char buffer[(2 * sizeof(sockaddr_in))]{};
    const std::function<void(std::unique_ptr<blitz::Connection>)>& callback;

    explicit accept_context(const std::function<void(std::unique_ptr<blitz::Connection>)>& callback) : io_context(OperationType::OP_ACCEPT), callback(callback) {
        // accept_socket will be assigned before posting AcceptEx
    }
};
#endif

namespace blitz {
    Provider::Provider(const size_t worker_threads, const size_t buffer_size) : servers(), buffer_size(buffer_size) {
        #ifdef PLATFORM_WINDOWS
            WSADATA wsa_data;
            if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
                WSACleanup();
                throw std::runtime_error("WSAStartup failed: " + WSAGetLastError());
            }

            instance = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 0);
            if (instance == nullptr) {
                WSACleanup();
                throw std::runtime_error("CreateIoCompletionPort failed: " + WSAGetLastError());
            }

            for (auto i = 0; i < worker_threads; ++i) {
                workers.emplace_back([&]() {
                    OVERLAPPED *overlapped;
                    ULONG_PTR key;
                    DWORD bytes;

                    while (!closed.load()) {
                        const auto result = GetQueuedCompletionStatus(instance, &bytes, &key, &overlapped, INFINITE);
                        const auto context = reinterpret_cast<io_context*>(overlapped);

                        switch (context->type) {
                            case OperationType::OP_ACCEPT: {
                                const auto accept = static_cast<accept_context*>(context);
                                break;
                            }
                            case OperationType::OP_CONNECT:
                                break;
                            default:
                                throw std::runtime_error(std::format("Unknown IOCP op: {}", static_cast<int>(context->type)));
                        }
                    }
                });
            }
        #elif defined(PLATFORM_LINUX)
            std::cout << "Runnign on linux" << std::endl;
        #endif
    }

    Provider::~Provider() {
        close();
    }

    void Provider::accept(const Address& server_address, const std::function<void(std::unique_ptr<Connection>)>& callback) {
        sockaddr_in addr = {};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(server_address.port());

        #ifdef PLATFORM_WINDOWS
            addr.sin_addr.s_addr = inet_addr(server_address.host().c_str());
            if (addr.sin_addr.s_addr == INADDR_NONE) {
                throw std::runtime_error("Invalid IP address: " + server_address.host());
            }

            socket_t server_fd;
            {
                std::lock_guard lock(server_mutex);
                if (const auto it = servers.find(server_address); it == servers.end()) {
                    server_fd = it->second;
                } else {
                    server_fd = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, WSA_FLAG_OVERLAPPED);
                    if (server_fd == INVALID_SOCKET) {
                        throw std::runtime_error("WSAListenSocket failed: " + std::to_string(WSAGetLastError()));
                    }

                    if (bind(server_fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == SOCKET_ERROR) {
                        closesocket(server_fd);
                        throw std::runtime_error("Bind failed: " + std::to_string(WSAGetLastError()));
                    }

                    if (listen(server_fd, SOMAXCONN) == SOCKET_ERROR) {
                        closesocket(server_fd);
                        throw std::runtime_error("Listen failed: " + std::to_string(WSAGetLastError()));
                    }

                    if (!CreateIoCompletionPort(reinterpret_cast<HANDLE>(server_fd), instance, server_fd, 0)) {
                        closesocket(server_fd);
                        throw std::runtime_error("Failed to associate socket with IOCP: " + std::to_string(GetLastError()));
                    }

                    servers[server_address] = server_fd;
                }
            }

            const auto accept_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, WSA_FLAG_OVERLAPPED);
            const auto context = accept_context(callback, accept_socket);
            const auto result = AcceptEx(server_fd, accept_socket, )


        #elif defined(PLATFORM_LINUX)
            // here
        #endif
    }

    void Provider::connect(const Address& server_address, const std::function<void(std::unique_ptr<Connection>)>& callback) {
        #ifdef PLATFORM_WINDOWS
            // here
        #elif defined(PLATFORM_LINUX)
            // here
        #endif
    }

    bool Provider::close() {
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