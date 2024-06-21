#include <util/types.h>
#include <algorithm>
#include <boost/asio.hpp>
#include <boost/asio/buffers_iterator.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/system/detail/error_code.hpp>
#include <memory>
#include <sstream>

#ifndef ULTRON_TCP_PROTOCOL_RESOLUTION
namespace asio = boost::asio;

using asio::awaitable;
using asio::use_awaitable;

namespace ultron {

template <typename Io>
class ProtocolResolution
    : public std::enable_shared_from_this<ProtocolResolution<Io>> {
 public:
  ProtocolResolution(Io io, logger_ptr logger) :
      io_(std::move(io)), logger_(logger->clone("resolution")) {}

  ~ProtocolResolution() { logger_->warn("Protocol resolution destroyed"); }

 public:
  awaitable<void> operator()();

  void test(asio::const_buffer const&);

  void terminate();

 private:
  Io io_;
  asio::streambuf sb_;
  logger_ptr logger_;
  std::stringstream ss_;
};

}  // namespace ultron
#endif  // !ULTRON_TCP_PROTOCOL_RESOLUTION
