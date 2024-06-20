#include <gtest/gtest.h>
#include <boost/asio/detached.hpp>
#include <core/event.hpp>
#include <string>

using namespace ultron;

class TestCoreEvent : public ::testing::Test {};

TEST_F(TestCoreEvent, type_propagation) {
  Event evt("some");

  EXPECT_EQ(evt.type(), "some");
}
