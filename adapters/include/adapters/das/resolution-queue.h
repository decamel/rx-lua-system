#include <util/asio.h>
#include <optional>
#include <queue>
#include <string>

#ifndef ULTRON_ADAPTERS_DAS_RESOLUTION_QUEUE_H
#define ULTRON_ADAPTERS_DAS_RESOLUTION_QUEUE_H

namespace ultron { namespace adapters { namespace queue {

struct sep_ts_queue {
 public:
  sep_ts_queue() : seek_offset_(0), income_({}) {}

 public:
  std::size_t flush(asio::const_buffer const& buf);

  std::optional<std::string> resolve(asio::streambuf& sb, std::size_t n);

  bool empty() const noexcept { return income_.has_value(); }

 private:
  std::size_t seek_offset_;
  std::optional<std::string> income_;
};

}}}  // namespace ultron::adapters::queue

#endif  // !ULTRON_ADAPTERS_DAS_RESOLUTION_QUEUE_H
