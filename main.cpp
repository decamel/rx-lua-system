#include <cassert>
#include <cctype>
#include <csignal>
#include <type_traits>
#define SOL_ALL_SAFETIES_ON 1

#include <spdlog/spdlog.h>
#include <lua.hpp>
#include <rxcpp/rx.hpp>
#include <sol/sol.hpp>

#include <boost/asio.hpp>

#include <tcp/server.h>
#include <adapters/adapters.hpp>

#include <util/types.h>
#include <log/log-action.hpp>

#include <engine/operator.h>

using namespace ultron;

struct tag {};

struct das_server_tag {};

template <typename Tag>
struct language_member {
  template <class... An>
  static auto member(An&&...) ->
      typename Tag::template include_header<std::false_type> {
    return typename Tag::template include_header<std::false_type>();
  }
};

template <>
struct language_member<das_server_tag> {
  template <class Port,
            class Enabled = std::enable_if<std::is_arithmetic<Port>::value>>
  static auto member(Port&& port, logger_ptr logger) {
    return tcp::server<adapters::tcp::das>(port, logger);
  }
};

int main(int argc, char* argv[]) {
  boost::asio::io_context ioc;
  boost::asio::signal_set termsig(ioc, SIGINT, SIGTERM);

  auto logger = spdlog::default_logger();
  logger->set_pattern("[%=15n] %^[%L]%$ %v");
  logger->set_level(spdlog::level::debug);

  sol::state lua;

  lua.new_usertype<LoggerProxy>(
      "Logger", sol::meta_function::construct,
      sol::factories(
          [logger](std::string const& name)
          { return std::make_shared<LoggerProxy>(logger->clone(name)); }),
      "info", &LoggerProxy::info);

  lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::io,
                     sol::lib::table);

  std::string script_path = "./main.lua";
  sol::load_result script = lua.load_file(script_path);

  if (!script.valid()) {
    logger->error("Failed to load LUA script file at `{}`", script_path);
    return -1;
  }

  auto result = script();
  if (result.status() != sol::call_status::ok) {
    sol::error err = result;
    logger->error("Failed to run script: {}", err.what());
    return -1;
  }

  auto server = language_member<das_server_tag>{}.member(
      7000, logger->clone("TCP SERVER"));

  termsig.async_wait(
      [&server, &ioc](auto const& e, int sig)
      {
        server.stop();
        ioc.stop();
      });

  server.run();

  ioc.run();

  return 0;
}
