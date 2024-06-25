#include <engine/operator.h>

namespace ultron {
Operator::Operator(std::string const& name) : name_(name) {}

void Operator::set_routes(std::vector<std::string> const& range) noexcept {
  routes = range;
}

void Operator::set_next(
    std::weak_ptr<sol::protected_function> const& v) noexcept {
  next_handler = v;
}

void Operator::set_source(rxcpp::subjects::subject<std::string> ss) noexcept {
  s = ss.get_observable().subscribe([this](std::string value)
                                    { on_next(std::move(value)); });
}

void Operator::on_next(std::string value) {
  if (next_handler.expired())
    return;

  auto sp = next_handler.lock();
  sp->call<void>(value);
}

}  // namespace ultron
