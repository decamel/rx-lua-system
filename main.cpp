#include <spdlog/common.h>
#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>
#include <cassert>
#include <csignal>
#include <operators/rx-observe_on.hpp>
#include <sol/optional_implementation.hpp>
#include <sol/table.hpp>
#include <subjects/rx-subject.hpp>
#include <thread>
#include <utility>
#define SOL_ALL_SAFETIES_ON 1

#include <spdlog/spdlog.h>
#include <lua.hpp>
#include <rxcpp/rx.hpp>
#include <sol/sol.hpp>

#include <boost/asio.hpp>

#include <tcp/server.h>

using namespace ultron;

class proc_signals : public rxcpp::subjects::subject<int> {
 public:
  template <typename... Sn>
  proc_signals(boost::asio::io_context& ioc, Sn... sig) :
      rxcpp::subjects::subject<int>(),
      ioc_(ioc),
      sigset_(ioc, std::forward<Sn>(sig)...) {}

  void wait() {
    sigset_.async_wait([this](auto const& e, int sig)
                       { get_subscriber().on_next(sig); });
  }

 private:
  boost::asio::io_context& ioc_;
  boost::asio::signal_set sigset_;
};

int main(int argc, char* argv[]) {
  boost::asio::io_context ioc;
  proc_signals rx_term(ioc, SIGINT, SIGTERM);

  auto logger = spdlog::default_logger();
  logger->set_level(spdlog::level::debug);

  tcp::server server(7000, logger->clone("TCP SERVER"));

  server.get_observable().subscribe(
      [logger](std::shared_ptr<tcp::session> sn)
      { logger->info("Handled propagated connection"); });

  rx_term.wait();
  rx_term.get_observable()
      .observe_on(rxcpp::observe_on_new_thread())
      .subscribe([logger](int s) { logger->info("Sig: {}", s); });

  server.run(tcp::run_location::detached);
  ioc.run();

  // std::string script_path = "./examples/main.lua";
  //
  // sol::state lua;
  // lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::io);
  // sol::load_result script = lua.load_file(script_path);
  //
  // if (!script.valid()) {
  //   logger->error("Failed to load LUA script file at `{}`", script_path);
  // }
  //
  // sol::optional<sol::table> result = script();
  //
  // assert(result->valid() && "Lua Script must return table");
  //
  // std::string name = result.value()["name"];
  // logger->info("Loaded module with name `{:10s}`", name);

  return 0;
}
