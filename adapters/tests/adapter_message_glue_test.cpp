#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <spdlog/common.h>
#include <spdlog/spdlog.h>
#include <adapters/adapters.hpp>
#include <boost/asio.hpp>
#include <boost/asio/streambuf.hpp>
#include <ostream>

using namespace ultron;

class TestDasAdapterGlue : public ::testing::Test {};

TEST_F(TestDasAdapterGlue, test_bufferisation) {

  auto logger = spdlog::default_logger()->clone("muted");

  logger->set_level(spdlog::level::off);

  asio::io_context ioc;

  asio::ip::tcp::socket socket(ioc);

  adapters::tcp::das adaptee(std::move(socket), logger);

  asio::streambuf writebuf;

  std::ostream ss(&writebuf);

  ss << "<DSSSD,1,1,data\0";
}
