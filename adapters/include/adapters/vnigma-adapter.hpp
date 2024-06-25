#include <spdlog/spdlog.h>
#include <util/asio.h>
#include <util/types.h>
#include <boost/asio/buffer.hpp>
#include <boost/asio/buffered_stream.hpp>
#include <boost/asio/buffers_iterator.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/redirect_error.hpp>
#include <iostream>
#include <istream>
#include <memory>

#ifdef USE_NULL_TERMINATOR
constexpr char das_delimeter = '\0';
#else
constexpr char das_delimeter = '\0';
#endif  // !USE_

#ifndef ULTRON_ADAPTERS_DAS_HPP
#define ULTRON_ADAPTERS_DAS_HPP

namespace ultron { namespace adapters {

constexpr std::size_t das_max_buffer_size = 2048;

using namespace coro;

namespace detail {

struct streambuf_message_cutter {
 public:
  std::size_t absorb(asio::streambuf& sb_, std::size_t nbytes) {
    sb_.commit(nbytes);

    auto buf = sb_.data();
    auto begin = asio::buffers_begin(buf);
    auto it = asio::buffers_begin(buf);
    auto end = asio::buffers_end(buf);

    while (it != end) {
      if ((*it) == '\0') {
        std::cout << "Gotcha!" << std::endl;
      }
      ++it;
    }

    auto data = std::string(it, end);

    // while (it != end) {
    //   std::cout << std::string(it, end) << std::endl;
    //   ++it;
    // }

    return 0;
  }
};

template <typename Io>
class das : public std::enable_shared_from_this<das<Io>> {

 public:
  das(Io io, logger_ptr logger) :
      io_(std::move(io)), sb_(das_max_buffer_size), logger_(logger) {}

  ~das() { logger_->warn("Destroyed"); }

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
      resolve(nbytes);

      std::string debug(asio::buffers_begin(sb_.data()),
                        asio::buffers_end(sb_.data()));
    }

    co_return;
  }

  void terminate() { io_.close(); }

 private:
  std::size_t flush() {
    if (seek_offset == 0)
      return 0;

    auto buf = sb_.data();
    auto begin = asio::buffers_begin(buf);
    auto cut_pos = begin + seek_offset;

    std::string data(begin, cut_pos);
    logger_->info("Received message `{}`", data);
    return cut_pos - begin;
  }

  void resolve(std::size_t n) {
    if (n == 0)
      return;

    auto right_border = seek_offset + n;

    sb_.commit(n);  // move nbytes from the output aka data from socket
                    // to the back of input sequence

    auto buf = sb_.data();

    auto begin = asio::buffers_begin(buf);
    auto it = asio::buffers_begin(buf);
    auto end = asio::buffers_end(buf);

    while (it != end && seek_offset < right_border) {
      ++seek_offset;

      if (*it == '\0') {
        std::size_t len = flush();
        sb_.consume(len);
        seek_offset = 0;
      }

      ++it;
    }
    //
    // logger_->info("Received {} bytes; Distance btw its {} bytes", n,
    //               end - begin);
    // logger_->info("First symbol is {}", *begin);
    // logger_->info("Last symbol is {}",
    //               *it == '\0' ? "null" : std::string({*it}));

    // auto begin = asio::buffers_begin(buf);
    // auto it = begin + seek_offset;
    // auto end = begin + seek_offset + n;
    //
    // if (*end == '\0') {
    //   logger_->info("Found message end");
    // }
    //
    // while (it != end) {
    //   std::cout << *it << std::endl;
    //   ++it;
    // }

    //
    // TODO: Resolve issue with iterators
    //
    // do {
    //   if (*it == das_delimeter) {
    //     std::string data(begin, it);
    //     logger_->info("Received sentence `{}`", data);
    //     sb_.consume(it - begin + 1);
    //     buf = sb_.data();
    //     begin = asio::buffers_begin(buf);
    //     it = asio::buffers_begin(buf);
    //     end = asio::buffers_end(buf);
    //     if (it == end)
    //       break;
    //   }
    //   ++it;
    // } while (it != end);

    seek_offset = asio::buffers_end(buf) - asio::buffers_begin(buf) - 1;
  }

  std::string escape(char c) {
    if (c == '\n')
      return "\\n";
    if (c == '\r')
      return "\\r";
    std::string s;
    s += c;
    return s;
  }

 private:
  Io io_;
  asio::streambuf sb_;
  logger_ptr logger_;
  std::size_t seek_offset;
};

}  // namespace detail

}}  // namespace ultron::adapters

#endif  // !ULTRON_ADAPTERS_DAS_HPP
