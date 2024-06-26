#include <adapters/das/resolution-queue.h>
#include <spdlog/spdlog.h>
#include <util/asio.h>
#include <util/types.h>
#include <boost/asio/async_result.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/streambuf.hpp>
#include <memory>
#include "vnigma/message/message_variant.hpp"

#if !defined(ULTRON_ADAPTERS_DAS_HPP)
#define ULTRON_ADAPTERS_DAS_HPP

constexpr char das_delimeter = '\0';

namespace ultron { namespace adapters {

constexpr std::size_t das_max_buffer_size = 2048;

using namespace coro;

namespace detail {

template <typename Io>
class das : public std::enable_shared_from_this<das<Io>> {
 public:
  using this_type = das<Io>;

 public:
  das(Io io, logger_ptr logger) :
      io_(std::move(io)), sb_(das_max_buffer_size), rqueue(), logger_(logger) {}

  ~das() { logger_->warn("Destroyed"); }

 private:
  // ------------------------------ Public interface ------------------------------
 public:
  awaitable<void> operator()() {
    auto self = this->shared_from_this();
    boost::system::error_code ec;

    for (;;) {
      // --------------------------------- Read bytes ---------------------------------
      std::size_t nbytes = co_await io_.async_read_some(
          sb_.prepare(1024), asio::redirect_error(asio::use_awaitable, ec));

      // ------------------------------- Error handling -------------------------------
      if (ec && ec != asio::error::eof) {
        logger_->error("IODevice failure in DAS: {}", ec.message());
      }

      // ------------------------ Verify connection is stable ---------------------
      if (ec && (io_.available() + nbytes) == 0) {
        terminate();
        break;
      }

      // -------------------------------- Handle bytes --------------------------------
      auto data = rqueue.resolve(sb_, nbytes);

      if (data.has_value()) {
        logger_->info("Received message `{}`", data.value());
      }
    }

    co_return;
  }

  void terminate() { io_.close(); }

 private:
  struct detect_task {
    typedef boost::system::error_code error;
    using handler = void(error);

    this_type& adapter;
    logger_ptr logger;

    template <typename Self>
    void operator()(Self& self) {

      if (adapter.rqueue.empty())
        self.compelete();

      logger->info("Detection task executed");
      self.complete();
    }
  };

  template <
      asio::completion_token_for<typename detect_task::handler> CompletionToken>
  auto async_detect(CompletionToken&& token) ->
      typename asio::async_result<typename std::decay<CompletionToken>::type,
                                  typename detect_task::handler>::return_type {
    return asio::async_compose<CompletionToken, void()>(
        detect_task{*this, logger_}, token);
  }

 private:
  Io io_;
  asio::streambuf sb_;
  adapters::queue::sep_ts_queue rqueue;
  logger_ptr logger_;
  enum { detect, configure, ready } state_ = detect;
};

}  // namespace detail

}}  // namespace ultron::adapters

#endif  // !ULTRON_ADAPTERS_DAS_HPP
