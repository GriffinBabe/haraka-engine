#pragma once
#include <boost/asio.hpp>

class Server
{
public:
    Server();

    ~Server();

private:

    /**
     * Blocking function that will start all the working threads of the server
     * and then join them once their work is done.
     */
    bool start_threads(std::uint16_t thread_count);

    void worker_thread(std::shared_ptr<boost::asio::io_service> service);

    std::shared_ptr<boost::asio::io_service> _service;
    std::shared_ptr<boost::asio::io_service::work> _work;
};