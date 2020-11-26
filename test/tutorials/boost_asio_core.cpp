#include <iostream>
#include <thread>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

std::mutex mutex;

void worker_thread(std::shared_ptr<boost::asio::io_service> service) {
    mutex.lock();
    std::cout << "[" << std::this_thread::get_id() << "] Begin of worker thread" << std::endl;
    mutex.unlock();

    boost::system::error_code ec;

    while (true) {
        try {
            service->run(ec);
            if (ec) { // this is a system exception, so we break out of the loop to finish the thread from the thread pool.
                mutex.lock();
                std::cout << "[" << std::this_thread::get_id() << "] Error code returned: " << ec << std::endl;
                mutex.unlock();
            }
            break;
        }
        catch (std::exception const &exc) { // this is a context exception, so we keep our thread active in the thread pool.
            mutex.lock();
            std::cout << "[" << std::this_thread::get_id() << "] Exception occurred: " << exc.what() << "."
                      << std::endl;
            mutex.unlock();
        }

    }

    mutex.lock();
    std::cout << "[" << std::this_thread::get_id() << "] End of worker thread" << std::endl;
    mutex.unlock();
}

size_t fib(size_t n) {
    if (n <= 1) {
        return n;
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
    return fib(n - 1) + fib(n - 2);
}

void calculate_fib(size_t n) {
    mutex.lock();
    std::cout << "[" << std::this_thread::get_id() << "] Starting calculating fibonacci for n = " << n << std::endl;
    mutex.unlock();

    size_t val = fib(n);

    mutex.lock();
    std::cout << "[" << std::this_thread::get_id() << "] Fibonacci result for " << n << " is " << val << std::endl;
    mutex.unlock();
}

void dispatch(int x) {
    mutex.lock();
    std::cout << "[" << std::this_thread::get_id() << "] " << __FUNCTION__ << " x= " << x << std::endl;
    mutex.unlock();
}

void post(int x) {
    mutex.lock();
    std::cout << "[" << std::this_thread::get_id() << "] " << __FUNCTION__ << " x= " << x << std::endl;
    mutex.unlock();
}

void run_3(std::shared_ptr<boost::asio::io_service> service) {
    for (int x = 0; x < 3; x++) {
        service->dispatch(boost::bind(&dispatch, x * 2));
        service->post(boost::bind(&post, x * 2 + 1));
    }
}

void print_num(int x) {
    // no mutex for the printing function here
    std::cout << "[" << std::this_thread::get_id() << "] x: " << x << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

void raise_exception(std::shared_ptr<boost::asio::io_service> service) {
    mutex.lock();
    std::cout << "[" << std::this_thread::get_id() << "] " << __FUNCTION__ << " called." << std::endl;
    mutex.unlock();

    service->post(boost::bind(&raise_exception, service));

    throw std::runtime_error("Oppsie Daisy");
}

void timer_handler(boost::system::error_code const &error,
                   std::shared_ptr<boost::asio::deadline_timer> timer,
                   std::shared_ptr<boost::asio::io_service::strand> strand) {
    if (error) {
        mutex.lock();
        std::cout << "[" << std::this_thread::get_id() << "] Error code: " << error << std::endl;
        mutex.unlock();
    } else {
        std::cout << "[" << std::this_thread::get_id() << "] Time Handler " << std::endl;
        mutex.unlock();

        timer->expires_from_now(boost::posix_time::seconds(5));
        // _1 is a placeholder defined in boost::asio
        timer->async_wait(strand->wrap(boost::bind(&timer_handler, _1, timer, strand)));
    }
}

int main(int argc, char **argv) {
    const std::uint8_t threads = 2;

    auto service = std::make_shared<boost::asio::io_service>();

    // smart pointer work to service, we can quit later the service
    auto work = std::make_shared<boost::asio::io_service::work>(*service);

    // strand makes us sure that the given post and dispatch orders are done sequentially
    auto strand = std::make_shared<boost::asio::io_service::strand>(*service);

    mutex.lock();
    std::cout << "[" << std::this_thread::get_id() << "] Starting the program." << std::endl;
    mutex.unlock();

    std::vector<std::thread> thread_list;
    thread_list.reserve(threads);

    for (int i = 0; i < threads; i++) {
        thread_list.emplace_back(std::thread(boost::bind(&worker_thread, service)));
    }

    strand->post(boost::bind(&print_num, 1));
    strand->post(boost::bind(&print_num, 2));
    strand->post(boost::bind(&print_num, 3));
    strand->post(boost::bind(&print_num, 4));
    strand->post(boost::bind(&print_num, 5));

    auto timer = std::make_shared<boost::asio::deadline_timer>(*service);
    timer->expires_from_now(boost::posix_time::seconds(5));
    timer->async_wait(strand->wrap(boost::bind(&timer_handler, _1, timer, strand)));

    std::cin.get();

    // work::reset() will finish all the queued works before stopping
    // work.reset();

    // service::stop() will force stop all the queued works before stopping
    service->stop();

    for (std::thread &thread : thread_list) {
        thread.join();
    }

    // by destroying the work object, all remaining tasks will be executed by the service, then the service will stop

    std::cout << "Service has exited" << std::endl;

    return 0;
}