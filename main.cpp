#include <spdlog/common.h>
#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>
#include <cassert>
#include <cctype>
#include <csignal>
#include <operators/rx-observe_on.hpp>
#include <sol/optional_implementation.hpp>
#include <sol/property.hpp>
#include <sol/raii.hpp>
#include <sol/table.hpp>
#include <sol/types.hpp>
#include <type_traits>
#define SOL_ALL_SAFETIES_ON 1

#include <spdlog/spdlog.h>
#include <lua.hpp>
#include <rxcpp/rx.hpp>
#include <sol/sol.hpp>

#include <boost/asio.hpp>

#include <tcp/server.h>
#include <adapters/vnigma-adapter.hpp>

using namespace ultron;

struct tag {};

struct server_tag {};

template <typename Tag>
struct language_member {
  template <class... An>
  static auto member(An&&...) ->
      typename Tag::template include_header<std::false_type> {
    return typename Tag::template include_header<std::false_type>();
  }
};

template <>
struct language_member<server_tag> {
  template <class Port,
            class Enabled = std::enable_if<std::is_arithmetic<Port>::value>>
  static auto member(Port&& port, logger_ptr logger) {
    return tcp::server(port, logger);
  }
};

struct LuaTcpProxy {
  LuaTcpProxy(tcp::server& s, logger_ptr logger) :
      server_(s), logger_(logger){};

  ~LuaTcpProxy() { s.unsubscribe(); }

 public:
  sol::function get_next() const { return next_; }

  void set_next(sol::function f) {
    s.unsubscribe();
    next_ = f;
    s = server_.get_observable().subscribe([this](auto const&) { next_(); });
  }

 private:
  sol::function next_;
  tcp::server& server_;
  logger_ptr logger_;
  rxcpp::subscription s;
};

int main(int argc, char* argv[]) {
  boost::asio::io_context ioc;
  boost::asio::signal_set termsig(ioc, SIGINT, SIGTERM);

  auto logger = spdlog::default_logger();
  logger->set_level(spdlog::level::debug);

  // assert(result->valid() && "Lua Script must return table");

  // std::string name = result.value()["name"];
  // logger->info("Loaded module with name `{:10s}`", name);

  auto server =
      language_member<server_tag>{}.member(7000, logger->clone("TCP SERVER"));

  sol::state lua;

  sol::table module = lua.create_named_table("Traffic");
  module.new_usertype<LuaTcpProxy>(
      "tcp", sol::meta_function::construct,
      sol::factories([logger, &server]()
                     { return std::make_shared<LuaTcpProxy>(server, logger); }),
      "next", sol::property(&LuaTcpProxy::get_next, &LuaTcpProxy::set_next));

  lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::io,
                     sol::lib::table);

  std::string script_path = "./examples/main.lua";
  sol::load_result script = lua.load_file(script_path);

  if (!script.valid()) {
    logger->error("Failed to load LUA script file at `{}`", script_path);
    return -1;
  }

  auto result = script();
  assert(result.status() == sol::call_status::ok);

  server.get_observable().subscribe(
      [logger](std::shared_ptr<tcp::session> sn)
      { logger->info("Handled propagated connection"); });

  termsig.async_wait([&server](auto const& e, int sig) { server.stop(); });

  server.run<adapter::das<asio::ip::tcp::socket>>();
  ioc.run();

  return 0;
}
