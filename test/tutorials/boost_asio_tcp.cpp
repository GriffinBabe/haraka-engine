#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <iostream>
#include <mutex>
#include <string>

std::mutex stdout_mutex;

void worker_thread(std::shared_ptr<boost::asio::io_service> service)
{
    stdout_mutex.lock();
    std::cout << "[" << std::this_thread::get_id()
              << "] Started working thread." << std::endl;
    stdout_mutex.unlock();

    boost::system::error_code ec;
    while (true) {
        try {
            service->run(ec);
            if (ec) {
                stdout_mutex.lock();
                std::cout << "[" << std::this_thread::get_id()
                          << "] IO Boost error code: " << ec << std::endl;
                stdout_mutex.unlock();
            }
            break;
        }
        catch (std::exception const& exc) {
            stdout_mutex.lock();
            std::cout << "[" << std::this_thread::get_id()
                      << "] Exception catched: " << exc.what() << std::endl;
            stdout_mutex.unlock();
        }
    }
}

void connect_socket(std::shared_ptr<boost::asio::io_service> service,
                    std::shared_ptr<boost::asio::ip::tcp::socket> socket,
                    std::string const& address,
                    int port)
{
    try {
        boost::asio::ip::tcp::resolver resolver(*service);
        boost::asio::ip::tcp::resolver::query query(address,
                                                    std::to_string(port));

        boost::asio::ip::tcp::resolver::iterator iterator =
            resolver.resolve(query);
        boost::asio::ip::tcp::endpoint endpoint = *iterator;

        stdout_mutex.lock();
        std::cout << "[" << std::this_thread::get_id()
                  << "] Connecting to: " << endpoint << std::endl;
        stdout_mutex.unlock();

        socket->connect(endpoint);
    }
    catch (std::exception const& exc) {
        stdout_mutex.lock();
        std::cout << "[" << std::this_thread::get_id()
                  << "] Exception catched: " << exc.what() << std::endl;
        stdout_mutex.unlock();
    }
}

void disconnect_socket(std::shared_ptr<boost::asio::ip::tcp::socket> socket)
{
    boost::system::error_code ec;
    socket->shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
    socket->close(ec);

    stdout_mutex.lock();
    if (ec) {
        std::cout << "[" << std::this_thread::get_id()
                  << "] Error code returned while closing socket: " << ec
                  << std::endl;
    }
    else {
        std::cout << "[" << std::this_thread::get_id() << "] Closed socket."
                  << std::endl;
    }
    stdout_mutex.unlock();
}

/**
 * Boost handler for on sent
 */
void on_sent(boost::system::error_code const& ec, std::size_t bytes_transfered)
{
    if (ec) {
        stdout_mutex.lock();
        std::cout << "[" << std::this_thread::get_id()
                  << "] Error code returned while sending bytes: " << ec
                  << std::endl;
        stdout_mutex.unlock();
    }
    else {
        stdout_mutex.lock();
        std::cout << "[" << std::this_thread::get_id() << "] Send "
                  << bytes_transfered << " bytes to client." << std::endl;
        stdout_mutex.unlock();
    }
}

void on_accept(boost::system::error_code const& ec,
               std::shared_ptr<boost::asio::ip::tcp::socket> socket)
{
    if (ec) {
        stdout_mutex.lock();
        std::cout << "[" << std::this_thread::get_id()
                  << "] Error code returned while accepting connection: " << ec
                  << std::endl;
        stdout_mutex.unlock();
    }
    else {
        stdout_mutex.lock();
        std::cout << "[" << std::this_thread::get_id()
                  << "] Connection accepted." << std::endl;
        stdout_mutex.unlock();
        boost::system::error_code ec_2;
        char data[13] = "Hello world!";
        socket->async_send(boost::asio::buffer(&data, sizeof(char) * 13),
                           boost::bind(&on_sent, _1, _2));
    }
}

int main(int argc, char** argv)
{
    const int threads = 2;

    stdout_mutex.lock();
    std::cout << "[" << std::this_thread::get_id() << "] Starting program."
              << std::endl;
    stdout_mutex.unlock();

    auto service = std::make_shared<boost::asio::io_service>();
    auto work = std::make_shared<boost::asio::io_service::work>(*service);

    std::vector<std::thread> thread_pool;
    thread_pool.reserve(threads);

    for (int i = 0; i < threads; i++) {
        thread_pool.emplace_back(std::thread(&worker_thread, service));
    }

    auto acceptor = std::make_shared<boost::asio::ip::tcp::acceptor>(*service);
    auto socket = std::make_shared<boost::asio::ip::tcp::socket>(*service);

    try {
        boost::asio::ip::tcp::resolver resolver(*service);
        boost::asio::ip::tcp::resolver::query query("127.0.0.1",
                                                    std::to_string(7777));
        boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);

        acceptor->open(endpoint.protocol());
        acceptor->set_option(
            boost::asio::ip::tcp::acceptor::reuse_address(false));
        acceptor->bind(endpoint);
        acceptor->listen(boost::asio::socket_base::max_connections);
        acceptor->async_accept(*socket, boost::bind(&on_accept, _1, socket));

        stdout_mutex.lock();
        std::cout << "[" << std::this_thread::get_id()
                  << "] Listening on: " << endpoint << std::endl;
        stdout_mutex.unlock();
    }
    catch (std::exception const& exc) {
        stdout_mutex.lock();
        std::cout << "[" << std::this_thread::get_id()
                  << "] Exception thrown while listening: " << exc.what()
                  << std::endl;
        stdout_mutex.unlock();
    }

    stdout_mutex.lock();
    std::cout << "Press [enter] to close program." << std::endl;
    stdout_mutex.unlock();
    std::cin.get();

    boost::system::error_code ec;
    acceptor->close(ec);
    socket->shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
    socket->close();

    //    boost::asio::io_service::strand strand(*service);

    // using strand guarantee us that the socket will first open then close
    //    service->post(strand.wrap(boost::bind(&connect_socket, service,
    //    socket, "google.com", 80)));
    //    service->post(strand.wrap(boost::bind(&disconnect_socket, socket)));

    // this is a wrong usage
    //    service->post(boost::bind(&connect_socket, service, socket,
    //    "google.com", 80)); service->post(boost::bind(&disconnect_socket,
    //    socket));

    work.reset();

    for (int i = 0; i < threads; i++) {
        thread_pool[i].join();
    }

    stdout_mutex.lock();
    std::cout << "[" << std::this_thread::get_id() << "] Program finished."
              << std::endl;
    stdout_mutex.unlock();

    return 0;
}
