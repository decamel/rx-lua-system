#include <adapters/das/resolution-queue.h>
#include <boost/asio/streambuf.hpp>

namespace ultron { namespace adapters { namespace queue {

std::size_t sep_ts_queue::flush(asio::const_buffer const& buf) {
  if (seek_offset_ == 0)
    return 0;

  auto begin = asio::buffers_begin(buf);
  auto cut_pos = begin + seek_offset_;

  std::string data(begin, cut_pos);

  income_ = std::string(begin, cut_pos);

  return cut_pos - begin;
}

std::optional<std::string> sep_ts_queue::resolve(asio::streambuf& sb,
                                                 std::size_t n) {
  if (n == 0)
    return {};

  auto right_border = seek_offset_ + n;

  sb.commit(n);  // move nbytes from the output aka data from socket
                 // to the back of input sequence

  auto buf = sb.data();

  auto begin = asio::buffers_begin(buf);
  auto it = asio::buffers_begin(buf);
  auto end = asio::buffers_end(buf);

  while (it != end && seek_offset_ < right_border) {
    ++seek_offset_;

    if (*it == '\0') {
      std::size_t len = flush(sb.data());
      sb.consume(len);
      seek_offset_ = 0;
      return income_;
    }

    ++it;
  }

  return {};
}

}}}  // namespace ultron::adapters::queue
