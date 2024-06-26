#ifndef ULTRON_ADAPTERS_DAS_CONFIG_REPOSITORY_H
#define ULTRON_ADAPTERS_DAS_CONFIG_REPOSITORY_H

#include <cstddef>
#include <list>
#include <string>
#include <unordered_map>
#include "vnigma/message/message_variant.hpp"

namespace ultron { namespace adapters { namespace das {

using serial_number_t = std::string;

using command = vnigma::message_variant;
using commands_list = std::list<command>;

using configuration_map = std::unordered_map<serial_number_t, commands_list>;

class ConfigurationRepository {
 public:
  bool contains(serial_number_t const& sn) const noexcept;

  vnigma::buffer next(serial_number_t const& sn);

  /// @brief Adds configuration record to specified serial number
  /// Will merge similar configuration commands
  void insert(serial_number_t const& sn, command& cmd);

  /// @brief Returns number of devices to be configured
  std::size_t size() const;

  /// @brief Returns number of commands to be sent
  /// for specific device with provided serial number
  std::size_t size(serial_number_t const& sn) const;

  /// @brief UNIMPLEMENTDED Seals similar messages into single command
  ///
  [[maybe_unused]] void seal();

  /// @brief UNIMPLEMENTDED Seals similar messages for specific serial number into single command
  [[maybe_unused]] void seal(serial_number_t const& sn);

 private:
  configuration_map config_map_;
};

}}}  // namespace ultron::adapters::das

#endif  // !ULTRON_ADAPTERS_DAS_CONFIG_REPOSITORY_H
