#include <adapters/das/configuration-repository.h>
#include "vnigma/message/message_traits.hpp"
#include "vnigma/util/variant.h"

namespace ultron { namespace adapters { namespace das {

bool ConfigurationRepository::contains(
    serial_number_t const& sn) const noexcept {
  return config_map_.contains(sn);
}

vnigma::buffer ConfigurationRepository::next(serial_number_t const& sn) {
  if (!contains(sn))
    return "";

  auto item = config_map_.at(sn).front();
  vnigma::buffer buf = item.visit(vnigma::overload{
      [](auto const& v) -> vnigma::buffer { return v.as_buffer(); },
      [](vnigma::system_error const& e) -> vnigma::buffer
      {
        return "";
      }});
  config_map_.at(sn).pop_front();
  return buf;
}

void ConfigurationRepository::insert(serial_number_t const& sn, command& cmd) {
  if (config_map_.contains(sn) == false) {
    config_map_.emplace(sn, commands_list{});
  }

  config_map_.at(sn).push_back(cmd);
}

void ConfigurationRepository::seal() {
  for (auto entry : config_map_) {
    serial_number_t sn = std::get<0>(entry);
    seal(sn);
  }
}
void ConfigurationRepository::seal(serial_number_t const& sn) {
  commands_list& ll = config_map_.at(sn);
  for (auto it : ll) {}
}

std::size_t ConfigurationRepository::size(serial_number_t const& sn) const {
  if (!contains(sn))
    return 0;

  return config_map_.at(sn).size();
}
std::size_t ConfigurationRepository::size() const {
  return config_map_.size();
}

}}}  // namespace ultron::adapters::das
