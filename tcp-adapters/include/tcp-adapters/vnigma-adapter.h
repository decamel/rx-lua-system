#include <tcp/session.h>

namespace ultron { namespace adapter {

class das {
 public:
  das(tcp::session::shared);

 public:
  void resolve_seial_number();

 private:
  tcp::session::shared sn_;
};

}}  // namespace ultron::adapter
