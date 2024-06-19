#include <gtest/gtest.h>
#include <boost/asio/detached.hpp>
#include <core/event.hpp>
#include <core/origin.hpp>
#include <string>

using namespace ultron;

class TestCoreEvent : public ::testing::Test {};

TEST_F(TestCoreEvent, type_propagation) {
  Event evt("some");

  EXPECT_EQ(evt.type(), "some");
}

class TestOriginWrite : public ::testing::Test {};

TEST_F(TestOriginWrite, closed_by_default) {
  Origin og;

  EXPECT_FALSE(og.is_opened());
}

TEST_F(TestOriginWrite, write_buffer) {
  Origin og;

  int i_data = 1;
  std::string s_data = "some data which emulated";
  og.write(s_data.c_str(), asio::detached);
  og.write(i_data, asio::detached);
}
