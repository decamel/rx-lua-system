#include <boost/asio.hpp>
#include <boost/system/detail/error_code.hpp>

namespace ultron {
namespace asio = boost::asio;

namespace coro {
using asio::awaitable;
using asio::co_spawn;
using asio::detached;
using asio::use_awaitable;
}  // namespace coro

}  // namespace ultron
