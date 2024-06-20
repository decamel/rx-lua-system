#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace ultron {

struct Component {
  void operator()() { std::cout << "hello from Component" << std::endl; }
};

struct BindingRequest {
  std::string match;
  std::weak_ptr<Component> component;
};

class Router {
 public:
  Router() : key_table_() {}

 public:
  void schedule_request(BindingRequest& req) {

    key_table_.try_emplace(req.match, std::vector<std::weak_ptr<Component>>(5));
    auto& ll = key_table_.at(req.match);

    ll.push_back(req.component);
  };

 private:
  std::unordered_map<std::string, std::vector<std::weak_ptr<Component>>>
      key_table_;
};

}  // namespace ultron
