#include <util/types.h>
#include <memory>

#include <boost/asio.hpp>

namespace asio = boost::asio;

namespace ultron { namespace tcp {

class session : std::enable_shared_from_this<session> {
 public:
  using shared = std::shared_ptr<session>;

 public:
  session(asio::ip::tcp::socket);

  ~session() { disconnect(); }

 public:
  void disconnect();
  void is_connected();

 public:
  bool write();

  void next();

 protected:
  asio::ip::tcp::socket socket_;
  logger_ptr logger_;
};

}}  // namespace ultron::tcp
