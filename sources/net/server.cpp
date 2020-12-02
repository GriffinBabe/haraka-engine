#include "net/server.hpp"
#include <iostream>
#include <thread>

net::ServerInterface::ServerInterface()
{
    _service = std::make_shared<boost::asio::io_service>();
    _work = std::make_shared<boost::asio::io_service::work>(*_service);
}

net::ServerInterface::~ServerInterface()
{
}

void net::ServerInterface::worker_thread(
    std::shared_ptr<boost::asio::io_service> service)
{
    std::cout << "[" << std::this_thread::get_id()
              << "] Started working thread." << std::endl;

    boost::system::error_code ec;
    while (true) {
        try {
            service->run(ec);
            if (ec) {
                std::cout << "[" << std::this_thread::get_id()
                          << "] IO Boost error code: " << ec << std::endl;
            }
            break;
        }
        catch (std::exception const& exc) {
            std::cout << "[" << std::this_thread::get_id()
                      << "] Exception catched: " << exc.what() << std::endl;
        }
    }
}

bool net::ServerInterface::start_threads(std::uint16_t thread_count)
{
    std::vector<std::thread> threads;
    threads.reserve(thread_count);

    for (std::uint16_t i = 0; i < thread_count; i++) {
        threads.emplace_back(
            std::thread(&ServerInterface::worker_thread, this, _service));
    }

    for (std::uint16_t i = 0; i < thread_count; i++) {
        threads[i].join();
    }
}
