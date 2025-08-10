// #include <winsock2.h>
// #include <mswsock.h>
// #include <windows.h>
// #include <iostream>
// #include <stdexcept>
// #include <thread>
// #include <chrono>

// #pragma comment(lib, "ws2_32.lib")
//
// enum class OperationType : uint8_t {
//     OP_READ,
//     OP_WRITE,
//     OP_ACCEPT,
//     OP_CONNECT
// };
//
// struct io_context {
//     virtual ~io_context() = default;
//     OVERLAPPED overlapped;
//     OperationType type;
// };
//
// struct accept_context : public io_context {
//     SOCKET accept_socket = INVALID_SOCKET;
//
//     accept_context() {
//         type = OperationType::OP_ACCEPT;
//     }
// };
//
//
// struct read_context {
//     OVERLAPPED overlapped;
//     WSABUF wsa_buf;
//     char buffer[1024];
//     SOCKET socket;
//     enum OperationType { OP_READ, OP_ACCEPT } operation_type;
// };
//
// void server_thread_func() {
//     WSADATA wsa_data;
//     if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
//         std::cerr << "WSAStartup failed" << std::endl;
//         return;
//     }
//
//     // Create IOCP handle
//     const auto handle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 0);
//     if (handle == nullptr) {
//         std::cerr << "CreateIoCompletionPort failed" << std::endl;
//         WSACleanup();
//         return;
//     }
//
//     // Create listen socket
//     const auto listen_sock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, WSA_FLAG_OVERLAPPED);
//     if (listen_sock == INVALID_SOCKET) {
//         std::cerr << "Socket creation failed" << std::endl;
//         CloseHandle(handle);
//         WSACleanup();
//         return;
//     }
//
//     sockaddr_in addr = {};
//     addr.sin_family = AF_INET;
//     addr.sin_addr.s_addr = INADDR_ANY;
//     addr.sin_port = htons(9000);
//
//     if (bind(listen_sock, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == SOCKET_ERROR) {
//         std::cerr << "Bind failed: " << WSAGetLastError() << std::endl;
//         closesocket(listen_sock);
//         CloseHandle(handle);
//         WSACleanup();
//         return;
//     }
//
//     if (listen(listen_sock, SOMAXCONN) == SOCKET_ERROR) {
//         std::cerr << "Listen failed: " << WSAGetLastError() << std::endl;
//         closesocket(listen_sock);
//         CloseHandle(handle);
//         WSACleanup();
//         return;
//     }
//
//     std::cout << "Server listening on port 9000..." << std::endl;
//
//     // Accept a client connection (blocking)
//     sockaddr_in client_addr;
//     int client_addr_len = sizeof(client_addr);
//     SOCKET client_sock = accept(listen_sock, (sockaddr*)&client_addr, &client_addr_len);
//     if (client_sock == INVALID_SOCKET) {
//         std::cerr << "Accept failed: " << WSAGetLastError() << std::endl;
//         closesocket(listen_sock);
//         CloseHandle(handle);
//         WSACleanup();
//         return;
//     }
//
//     std::cout << "Client connected!" << std::endl;
//
//     // Associate client socket with IOCP
//     if (CreateIoCompletionPort((HANDLE)client_sock, handle, (ULONG_PTR)client_sock, 0) == nullptr) {
//         std::cerr << "CreateIoCompletionPort association failed" << std::endl;
//         closesocket(client_sock);
//         closesocket(listen_sock);
//         CloseHandle(handle);
//         WSACleanup();
//         return;
//     }
//
//     // Post initial read
//     auto* ctx = new io_context{};
//     ctx->socket = client_sock;
//     post_read(ctx);
//
//     DWORD bytes_transferred;
//     ULONG_PTR completion_key;
//     LPOVERLAPPED overlapped;
//
//     while (true) {
//         std::cout << "Waiting for completion..." << std::endl;
//         const auto success = GetQueuedCompletionStatus(
//             handle,
//             &bytes_transferred,
//             &completion_key,
//             &overlapped,
//             INFINITE
//         );
//
//         if (!success) {
//             if (overlapped == nullptr) {
//                 std::cout << "Shutdown signal received." << std::endl;
//                 break;
//             }
//             const auto err = GetLastError();
//             std::cerr << "GetQueuedCompletionStatus failed: " << err << std::endl;
//             break;
//         }
//
//         if (bytes_transferred == 0 && overlapped != nullptr) {
//             std::cout << "Connection closed by client." << std::endl;
//
//             delete ctx;
//             continue;
//         }
//
//         const auto context = reinterpret_cast<io_context*>(overlapped);
//         switch (context->type) {
//             case OperationType::OP_ACCEPT: {
//                 const auto accept = static_cast<accept_context*>(context);
//
//                 break;
//             }
//
//             case OperationType::OP_CONNECT: {
//                 break;
//             }
//
//             case OperationType::OP_READ: {
//             }
//
//             case OperationType::OP_WRITE: {
//
//             }
//             default:
//                 throw std::runtime_error("Unknown operation type");
//         }
//     }
//
//     closesocket(listen_sock);
//     CloseHandle(handle);
//     WSACleanup();
// }
//
// void client_thread_func() {
//     // Wait a bit to let server start
//     std::this_thread::sleep_for(std::chrono::seconds(1));
//
//     WSADATA wsa_data;
//     if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
//         std::cerr << "Client WSAStartup failed" << std::endl;
//         return;
//     }
//
//     SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
//     if (sock == INVALID_SOCKET) {
//         std::cerr << "Client socket creation failed" << std::endl;
//         WSACleanup();
//         return;
//     }
//
//     sockaddr_in server_addr{};
//     server_addr.sin_family = AF_INET;
//     server_addr.sin_port = htons(9000);
//     server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
//
//     if (connect(sock, (sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
//         std::cerr << "Client connect failed: " << WSAGetLastError() << std::endl;
//         closesocket(sock);
//         WSACleanup();
//         return;
//     }
//
//     std::cout << "Client connected to server!" << std::endl;
//
//     const char* msg = "Hello from client!";
//     send(sock, msg, (int)strlen(msg), 0);
//
//     closesocket(sock);
//     WSACleanup();
// }

#include "blitz/provider.h"

int main() {
    auto provider = blitz::provider(1, 1);
    provider.close();
    return 0;
}
