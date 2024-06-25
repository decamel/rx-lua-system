#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <adapters/adapters.hpp>
#include <boost/asio.hpp>
#include <boost/asio/streambuf.hpp>
#include <ostream>

using namespace ultron;

class TestDasAdapterGlue : public ::testing::Test {};

TEST_F(TestDasAdapterGlue, test_bufferisation) {

  asio::streambuf sb;

  std::ostream ss(&sb);
  ss << "Hello";

  std::cout << "Current buffer size: " << sb.size()
            << std::endl;  // will show just 4 bytes instead of 9

  adapters::detail::streambuf_message_cutter mc{};

  mc.absorb(sb, 18);
}
