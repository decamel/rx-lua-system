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

}}  // namespace ultron::tcp
