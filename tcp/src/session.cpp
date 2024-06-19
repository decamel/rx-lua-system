#include <spdlog/spdlog.h>
#include <tcp/session.h>
#include <iostream>
#include <sstream>

namespace ultron { namespace tcp {

session::session(asio::ip::tcp::socket socket) :
    socket_(std::move(socket)),
    logger_(spdlog::default_logger()->clone("SOCKET")) {}

void session::disconnect() {

  std::stringstream ss;
  ss << socket_.remote_endpoint();
  logger_->warn("Socket {} closed", ss.str());

  socket_.close();
}

}}  // namespace ultron::tcp
