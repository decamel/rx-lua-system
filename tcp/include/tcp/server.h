#include <boost/asio.hpp>
#include <cstdint>
#include <memory>
#include <rxcpp/rx.hpp>
#include <sstream>
#include <thread>
#include <type_traits>

#include <util/types.h>

namespace asio = boost::asio;

namespace ultron { namespace tcp {

class session;

using server_subject = rxcpp::subjects::subject<std::shared_ptr<session>>;

template <typename Protocol,
          typename Enabled = std::enable_if<std::is_constructible<
              Protocol, asio::ip::tcp::socket, logger_ptr>::value>>
class server {
 public:
  using protocol_t = std::decay_t<Protocol>;
  using protocol_ptr = std::shared_ptr<protocol_t>;
  using this_type = server;
  using io_t = asio::ip::tcp::socket;

 public:
  server(uint16_t const port, logger_ptr logger) :
      self_(ioc_, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)),
      logger_(logger) {}

  virtual ~server() {
    if (ioc_.stopped())
      return;

    stop();
  }

 public:
  bool run() {
    logger_->info("Starting up the server on port {}",
                  self_.local_endpoint().port());
    try {
      co_spawn(ioc_, listen(), asio::detached);

      thread_ = std::thread([this]() { ioc_.run(); });
    }
    catch (std::exception& e) {
      logger_->error("Failed to run server due to: {}", e.what());
      return false;
    }
    return true;
  }

  void stop() {
    ioc_.stop();
    if (thread_.joinable()) {
      thread_.join();
    }
    logger_->warn("Server has been stopped");
  }

 private:
  asio::awaitable<void> listen() {

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
        logger_->error("Server connection acceptance error: {}", e.what());
        break;
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
