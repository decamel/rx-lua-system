#include <adapters/das/adapter.hpp>

namespace ultron { namespace adapters { namespace tcp {

/// @brief vnigma protocol adapter over tcp socket
typedef adapters::detail::das<asio::ip::tcp::socket> das;

}}}  // namespace ultron::adapters::tcp
