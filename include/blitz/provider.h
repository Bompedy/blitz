#pragma once

#include <memory>
#include <atomic>
#include <thread>
#include <vector>
#include <functional>
#include "temp.h"
#include "connection.h"


namespace blitz {
    struct provider {
        explicit provider(size_t workers, size_t buffer_size);
        ~provider();

        void accept(const address& server_address, const std::function<void(std::unique_ptr<connection>)>& callback);
        void connect(const address& server_address, const std::function<void(std::unique_ptr<connection>)>& callback);
        bool close();

    private:
        instance_t instance{};
        std::mutex server_mutex{};
        std::unordered_map<address, socket_t> servers;
        size_t buffer_size;
        std::atomic_bool closed;
        std::vector<std::thread> workers;
    };
}
