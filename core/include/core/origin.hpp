#include <boost/asio.hpp>
#include <boost/asio/compose.hpp>
#include <sstream>
#include <type_traits>

namespace asio = boost::asio;

namespace ultron {

struct dummy_task {
  template <typename Self>
  void operator()(Self& self) {
    self.complete();
  }
};

class Origin {
 public:
  Origin() = default;

  virtual ~Origin() = default;

 public:
  template <typename Message,
            asio::completion_token_for<void()> CompletionToken>
  auto write(Message&& data, CompletionToken&& token) ->
      typename asio::async_result<typename std::decay<CompletionToken>::type,
                                  void()>::return_type {
    return asio::async_compose<CompletionToken, void()>(dummy_task{}, token);
  }

  virtual bool is_opened() noexcept { return false; };
};

}  // namespace ultron
