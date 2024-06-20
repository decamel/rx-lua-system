#include <tcp/server.h>
#include <boost/asio/co_spawn.hpp>
#include <exception>
#include <sstream>

namespace ultron { namespace tcp {

server::server(uint16_t const port, logger_ptr logger) :
    self_(ioc_, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)),
    logger_(logger) {}

void server::stop() {
  ioc_.stop();
  if (thread_.joinable()) {
    thread_.join();
  }
  logger_->warn("Server has been stopped");
}

bool server::run(run_location::current_location) {
  logger_->info("Starting up the server on port {}",
                self_.local_endpoint().port());
  try {
    co_spawn(ioc_, [this]() { return listen(); }, asio::detached);

    ioc_.run();
  }
  catch (std::exception& e) {
    logger_->error("Failed to run server due to: {}", e.what());
    return false;
  }
  return true;
}

bool server::run(run_location::detached_location) {
  logger_->info("Starting up the server on port {}",
                self_.local_endpoint().port());
  try {
    co_spawn(ioc_, [this]() { return listen(); }, asio::detached);

    thread_ = std::thread([this]() { ioc_.run(); });
  }
  catch (std::exception& e) {
    logger_->error("Failed to run server due to: {}", e.what());
    return false;
  }
  return true;
}

asio::awaitable<void> server::listen() {
  while (true) {
    try {
      asio::ip::tcp::socket socket =
          co_await self_.async_accept(asio::use_awaitable);

      // Log new connection acceptance
      std::stringstream ss;
      ss << socket.remote_endpoint();

      logger_->info("Received new connection {}", ss.str());

      // Resolve protocol of new socket connection
      resolution_ptr pr;
      pr.reset(new resolution(std::move(socket), logger_));
      asio::co_spawn(ioc_, pr->operator()(), asio::detached);
    }
    catch (std::exception& e) {
    }
  }
}

}}  // namespace ultron::tcp
