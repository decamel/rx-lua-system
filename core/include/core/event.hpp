
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

namespace ultron {

class Event {
 public:
  Event(std::string const& type) { meta["type"] = type; }

 public:
  std::string type() const noexcept {
    return meta.at("type").get<std::string>();
  }

 private:
  std::string type_;
  json data;
  json meta;
};

}  // namespace ultron
