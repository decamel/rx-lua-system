#include <boost/asio.hpp>
#include <boost/asio/awaitable.hpp>

namespace asio = boost::asio;
using asio::awaitable;

namespace ultron { namespace adapter {

template <typename Io>
class das {

 public:
  das(Io io) : io_(std::move(io)) {}

 public:
  awaitable<void> schedule() {}

 private:
  Io io_;
};

}}  // namespace ultron::adapter
