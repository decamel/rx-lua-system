#include <spdlog/spdlog.h>
#include <util/asio.h>
#include <util/types.h>
#include <boost/asio/async_result.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/buffers_iterator.hpp>
#include <boost/asio/streambuf.hpp>
#include <memory>

#if !defined(ULTRON_ADAPTERS_DAS_HPP)
#define ULTRON_ADAPTERS_DAS_HPP

constexpr char das_delimeter = '\0';

namespace ultron { namespace adapters {

constexpr std::size_t das_max_buffer_size = 2048;

using namespace coro;

namespace detail {

// TODO: Apply CRTP to implement configuration functionality
// and the plain session
class das : public std::enable_shared_from_this<das> {
 public:
  using Io = asio::ip::tcp::socket;
  using this_type = das;

 public:
  das(Io io, logger_ptr logger) :
      io_(std::move(io)), sb_(das_max_buffer_size), logger_(logger) {}

  ~das() { logger_->warn("Destroyed"); }

 private:
  std::size_t consume_message(asio::const_buffer const& buffer,
                              std::string& target) {
    auto begin = asio::buffers_begin(buffer);
    auto it = asio::buffers_begin(buffer);
    auto end = asio::buffers_end(buffer);

    while (it != end) {
      if (*it == '\0') {
        target = std::string(begin, it);
        return it - begin + 1;
      }
      ++it;
    }
    return 0;
  }

  // ------------------------------ Public interface ------------------------------
 public:
  awaitable<void> operator()() {
    auto self = this->shared_from_this();
    boost::system::error_code ec;

    for (;;) {
      auto buffer = sb_.data();
      // -------------------------------- Handle bytes --------------------------------
      // in case if buffer already contains something
      std::string message("");
      // try to find termination symbol and make message
      std::size_t len = consume_message(buffer, message);

      // check how many bytes has been considered as message
      if (len) {
        sb_.consume(len);                                 // erase them
        logger_->info("Received message `{}`", message);  // forward message
        continue;  // make it consume all messages in latest trasmission
      }

      // --------------------------------- Read bytes ---------------------------------
      std::size_t nbytes = co_await io_.async_read_some(
          sb_.prepare(1024), asio::redirect_error(asio::use_awaitable, ec));

      // ------------------------------- Error handling -------------------------------
      sb_.commit(nbytes);

      if (ec && ec != asio::error::eof) {
        logger_->error("IODevice failure in DAS: {}", ec.message());
      }

      // ------------------------ Verify connection is stable ---------------------
      if (ec && (io_.available() + nbytes) == 0) {
        terminate();
        break;
      }
    }

    co_return;
  }

  void terminate() { io_.close(); }

 private:
  Io io_;
  asio::streambuf sb_;
  logger_ptr logger_;
  enum { detect, configure, ready } state_ = detect;
};

}  // namespace detail

}}  // namespace ultron::adapters

#endif  // !ULTRON_ADAPTERS_DAS_HPP
