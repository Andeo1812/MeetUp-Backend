
#include "AsyncServer.hpp"
#include <signal.h>
#include <utility>
#include <iostream>

namespace http {
namespace AsyncServer {

AsyncServer::AsyncServer(const std::string& address, const std::string& port,
                        const std::string& doc_root): io_context_(1),
                        signals_(io_context_),
                        acceptor_(io_context_),
                        connection_manager_(),
                        request_handler_(doc_root) {
    // Register to handle the signals that indicate when the server should exit.
    // It is safe to register for the same signal multiple times in a program,
    // provided all registration for the specified signal is made through Asio.
    signals_.add(SIGINT);
    signals_.add(SIGTERM);
    signals_.add(SIGQUIT);

    do_await_stop();

    // Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
    boost::asio::ip::tcp::resolver resolver(io_context_);
    boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(address, port).begin();
    acceptor_.open(endpoint.protocol());
    acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    acceptor_.bind(endpoint);
    acceptor_.listen();

    do_accept();
}

void AsyncServer::run() {
    // The io_context::run() call will block until all asynchronous operations
    // have finished. While the server is running, there is always at least one
    // asynchronous operation outstanding: the asynchronous accept call waiting
    // for new incoming connections.
    // io_context_.dispatch();
    io_context_.run();
}

void AsyncServer::do_accept() {
    acceptor_.async_accept(
        [this](boost::system::error_code ec, boost::asio::ip::tcp::socket socket) {
            // Check whether the server was stopped by a signal before this
            // completion handler had a chance to run.
            if (!acceptor_.is_open()) {
                return;
            }

            if (!ec) {
                connection_manager_.open_connection(std::make_shared<ClientConnection>(io_context_,
                    std::move(socket), connection_manager_, request_handler_));
            }

            do_accept();
        });
}

void AsyncServer::do_await_stop() {
    signals_.async_wait(
        [this](boost::system::error_code ec, int signo) {
            // The server is stopped by cancelling all outstanding asynchronous
            // operations. Once all operations have finished the io_context::run()
            // call will exit.
            if (signo == SIGINT) {
                std::cout << "GET SIGINT " << signo << std::endl;
            }

            if (signo == SIGTERM) {
                std::cout << "GET SIGTERM " << signo << std::endl;
            }

            if (signo == SIGQUIT) {
                std::cout << "GET SIGQUIT " << signo << std::endl;

                std::cout << "*DO LOGIC*" << std::endl;
                do_await_stop();
                return;
            }

            acceptor_.close();
            connection_manager_.stop_all();
        });
}

} // namespace AsyncServer
} // namespace http