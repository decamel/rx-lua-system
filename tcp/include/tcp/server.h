#include <boost/asio.hpp>
#include <cstdint>
#include <memory>
#include <rxcpp/rx.hpp>
#include <thread>

#include <util/types.h>

#include <tcp/location.h>

namespace asio = boost::asio;

namespace ultron { namespace tcp {

class session;

using server_subject = rxcpp::subjects::subject<std::shared_ptr<session>>;

class server : public server_subject {
 public:
  using this_type = server;

 public:
  server(uint16_t const port, logger_ptr);

  virtual ~server() { stop(); }

 public:
  bool run(run_location::current_location);
  bool run(run_location::detached_location);

  void stop();

  virtual void on_lost_connection() {}

 private:
  asio::awaitable<void> listen();
  asio::awaitable<void> dispatch(std::string_view);

 private:
  asio::io_context ioc_;
  std::thread thread_;

  asio::ip::tcp::acceptor self_;

  logger_ptr logger_;
};

}}  // namespace ultron::tcp
