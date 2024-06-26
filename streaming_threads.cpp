#include <spdlog/spdlog.h>
#include <unistd.h>
#include <util/asio.h>
#include <iostream>
#include <istream>
#include <ostream>
#include <thread>

using namespace ultron;

int main() {
  asio::io_context ioc;

  asio::streambuf ss;

  auto logger = spdlog::default_logger();
  logger->set_level(spdlog::level::debug);
  logger->set_pattern("[%t] %v");

  std::thread th1 = std::thread(
      [&ss, logger]()
      {
        logger->info("Started first thread");
        std::ostream os(&ss);
        for (int i; i < 10; i++) {
          sleep(1);
          os << i;
          logger->info("Sent {}", i);
        }
      });

  std::thread th2 = std::thread(
      [&ss, logger]()
      {
        logger->debug("Started second thread");
        std::istream is(&ss);
        for (int i; i < 10; ++i) {
          sleep(1);
          is >> i;
          logger->debug("Received {}", i);
        }
      });

  th1.join();
  th2.join();

  std::cout << "Flush: " << &ss << std::endl;

  return 0;
}
