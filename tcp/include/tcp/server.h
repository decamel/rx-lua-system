#include <boost/asio.hpp>
#include <cstdint>
#include <memory>
#include <rxcpp/rx.hpp>
#include <sstream>
#include <thread>

#include <util/types.h>

namespace asio = boost::asio;

namespace ultron { namespace tcp {

class session;

using server_subject = rxcpp::subjects::subject<std::shared_ptr<session>>;

class server : public server_subject {
 public:
  using this_type = server;
  using io_t = asio::ip::tcp::socket;

 public:
  server(uint16_t const port, logger_ptr);

  virtual ~server() {
    if (ioc_.stopped())
      return;

    stop();
  }

 public:
  template <typename Protocol,
            typename Enabled =
                std::enable_if<std::is_constructible<Protocol, io_t>::value>>
  bool run() {
    logger_->info("Starting up the server on port {}",
                  self_.local_endpoint().port());
    try {
      co_spawn(ioc_, [this]() { return listen<Protocol>(); }, asio::detached);

      thread_ = std::thread([this]() { ioc_.run(); });
    }
    catch (std::exception& e) {
      logger_->error("Failed to run server due to: {}", e.what());
      return false;
    }
    return true;
  }

  void stop();

 private:
  template <typename Protocol>
  asio::awaitable<void> listen() {

    using protocol_t = Protocol;
    using protocol_ptr = std::shared_ptr<protocol_t>;

    while (true) {
      try {
        asio::ip::tcp::socket socket =
            co_await self_.async_accept(asio::use_awaitable);

        // Log new connection acceptance
        std::stringstream ss;
        ss << socket.remote_endpoint();

        logger_->info("Received new connection {}", ss.str());
        // Create logger for new socket
        std::string name = ss.str();
        logger_ptr lp = logger_->clone(name);
        // Delegate socket processing to specified protocol
        protocol_ptr pr;
        pr.reset(new protocol_t(std::move(socket), lp));
        asio::co_spawn(ioc_, (*pr)(), asio::detached);
      }
      catch (std::exception& e) {
      }
    }
  }

 private:
  asio::io_context ioc_;
  std::thread thread_;

  asio::ip::tcp::acceptor self_;

  logger_ptr logger_;
};

}}  // namespace ultron::tcp
