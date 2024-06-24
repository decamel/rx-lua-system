#include <util/types.h>
#include <boost/asio.hpp>
#include <boost/asio/buffers_iterator.hpp>
#include <boost/asio/redirect_error.hpp>
#include <memory>

namespace asio = boost::asio;
using asio::awaitable;

namespace ultron { namespace adapter {

template <typename Io>
class das : public std::enable_shared_from_this<das<Io>> {

 public:
  das(Io io, logger_ptr logger) : io_(std::move(io)), logger_(logger) {}

  ~das() { logger_->warn("IO device destroyed"); }

 public:
  awaitable<void> schedule() {
    auto self = this->shared_from_this();
    std::string data;
    boost::system::error_code ec;
    for (;;) {

      std::size_t nbytes = co_await io_.async_read_some(
          buf_.prepare(1024), asio::redirect_error(asio::use_awaitable, ec));

      buf_.commit(nbytes);

      std::string data(asio::buffers_begin(buf_.data()),
                       asio::buffers_end(buf_.data()));

      buf_.consume(nbytes);

      logger_->warn(data);

      if (ec) {
        terminate();
        break;
      }
    }
  }

  void terminate() { io_.close(); }

 private:
  Io io_;
  asio::streambuf buf_;
  logger_ptr logger_;
};

}}  // namespace ultron::adapter
