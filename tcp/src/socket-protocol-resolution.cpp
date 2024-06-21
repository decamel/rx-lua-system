#include <tcp/protocol-resolution.h>
#include <boost/asio/buffers_iterator.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/redirect_error.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <ios>

using io = boost::asio::ip::tcp::socket;
namespace ultron {

template <>
void ProtocolResolution<io>::terminate() {
  io_.close();
}

template <>
void ProtocolResolution<io>::test(asio::const_buffer const& buf) {}

template <>
awaitable<void> ProtocolResolution<io>::operator()() {
  auto self = shared_from_this();
  std::stringstream addr;
  addr << io_.remote_endpoint();

  boost::system::error_code ec;

  logger_->debug("Okay, entered");

  for (std::string data;;) {
    assert((bool)ec == false);
    logger_->debug("Socket {} waiting to read", addr.str());
    // --------------------------------- Read bytes ---------------------------------
    std::size_t nbytes = co_await io_.async_read_some(
        sb_.prepare(1024),
        boost::asio::redirect_error(boost::asio::use_awaitable, ec));

    logger_->debug("Stream bytes read: {}. Left {}", nbytes, io_.available());
    // ---------- Verify if connection closed and everything read properly ----------
    if (!io_.is_open() && nbytes == 0) {
      break;
    }
    // ------------------ Log if error except of connection close ---------------
    if (ec && ec != asio::error::eof) {
      logger_->error("IODevice error in protocol resolution: {}", ec.message());
    }
    // -------------------------------- Handle bytes --------------------------------
    sb_.commit(nbytes);  // move nbytes from the output aka data from socket
                         // to the back of input sequence

    test(sb_.data());

    // sb_.consume(nbytes);  // erase nbytes from input sequence

    // ----------------------- Terminate if any error occured -----------------------
    if (ec && io_.available() == 0) {
      terminate();
      break;
    }
  }
  co_return;
}

}  // namespace ultron
