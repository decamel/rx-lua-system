#include <tcp/protocol-resolution.h>
#include <boost/asio/error.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/redirect_error.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <ios>
#include <istream>
#include <iterator>

namespace ip = asio::ip;

using io = boost::asio::ip::tcp::socket;
namespace ultron {

template <>
void ProtocolResolution<io>::terminate() {
  io_.close();
}

template <>
awaitable<void> ProtocolResolution<io>::operator()() {
  auto self = shared_from_this();
  std::stringstream addr;
  addr << io_.remote_endpoint();

  boost::system::error_code ec;

  logger_->debug("Okay, entered");

  for (;;) {
    assert((bool)ec == false);
    logger_->debug("Socket {} waiting to read", addr.str());
    // --------------------------------- Read bytes ---------------------------------
    std::size_t nbytes = co_await io_.async_read_some(
        sb_.prepare(1024),
        boost::asio::redirect_error(boost::asio::use_awaitable, ec));
    // ---------- Verify if connection closed and everything read properly ----------
    if (!io_.is_open() && nbytes == 0) {
      break;
    }
    // ------------------ Log if error except of connection close ---------------
    if (ec && ec != asio::error::eof) {
      logger_->error("IODevice error in protocol resolution: {}", ec.message());
    }
    // ----------------------- Terminate if any error occured -----------------------
    // -------------------------------- Handle bytes --------------------------------
    sb_.commit(nbytes);

    std::istream ss(&sb_);
    std::string data;
    ss >> data;
    logger_->debug("Received `{}` from {}", data, addr.str());

    if (ec && io_.available() == 0) {
      terminate();
      break;
    }
  }
  co_return;
}

}  // namespace ultron
