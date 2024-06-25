#include <memory>
#include <rxcpp/rx-lite.hpp>
#include <rxcpp/subjects/rx-subject.hpp>
#include <sol/forward.hpp>
#include <sol/protected_function.hpp>
#include <string>
#include <vector>

#ifndef ULTRON_LUA_ENGINE_OPERATOR_H
#define ULTRON_LUA_ENGINE_OPERATOR_H

namespace ultron {

/**
 * @brief Lua script C++ representation
 */
class Operator {
 public:
  Operator(std::string const& name);

  virtual ~Operator() { s.unsubscribe(); }

 public:
  /// @brief Lua iterested routes binding
  void set_routes(std::vector<std::string> const& range) noexcept;

  /// @brief Lua next handler setter
  void set_next(std::weak_ptr<sol::protected_function> const& v) noexcept;

  /// @brief Lua traffic source binding
  void set_source(rxcpp::subjects::subject<std::string> ss) noexcept;

  /// @brief Lua traffic source binding
  void on_next(std::string value);

 private:
  std::vector<std::string> routes;
  std::weak_ptr<sol::protected_function> next_handler;
  rxcpp::subscription s;
  std::string name_;
};

}  // namespace ultron

#endif  // !ULTRON_LUA_ENGINE_OPERATOR_H
