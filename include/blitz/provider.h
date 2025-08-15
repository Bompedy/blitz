#pragma once

#include <memory>
#include <atomic>
#include <thread>
#include <vector>
#include <functional>
#include "temp.h"
#include "Connection.h"


namespace blitz {
    struct Provider {
        explicit Provider(size_t workers, size_t buffer_size);
        ~Provider();

        void accept(const Address& server_address, const std::function<void(std::unique_ptr<Connection>)>& callback);
        void connect(const Address& server_address, const std::function<void(std::unique_ptr<Connection>)>& callback);
        bool close();

    private:
        instance_t instance{};
        std::mutex server_mutex{};
        std::unordered_map<Address, socket_t> servers;
        size_t buffer_size;
        std::atomic_bool closed;
        std::vector<std::thread> workers;
    };
}
