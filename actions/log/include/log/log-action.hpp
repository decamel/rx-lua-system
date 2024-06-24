#include <util/types.h>
#include <string>

namespace ultron {
struct LoggerProxy {
  LoggerProxy(logger_ptr logger) : logger_(logger) {}

  void info(std::string const& msg) const noexcept { logger_->info(msg); }

 private:
  logger_ptr logger_;
};

}  // namespace ultron
