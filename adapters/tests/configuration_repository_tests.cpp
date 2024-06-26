#include <adapters/das/configuration-repository.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <vnigma/types.h>
#include "vnigma/device.h"
#include "vnigma/message/das_set_frequency.h"

using namespace ultron;

class DasConfigRepoTests : public ::testing::Test {
 public:
  DasConfigRepoTests() : ::testing::Test(), device(1, vnigma::Type::analog) {}

 public:
  void SetUp() { repo = adapters::das::ConfigurationRepository(); }

 public:
  adapters::das::ConfigurationRepository repo;
  vnigma::device device;
};

TEST_F(DasConfigRepoTests, empty_by_default) {
  EXPECT_EQ(repo.size(), 0) << "Repository is not empty by default as expected";
}

TEST_F(DasConfigRepoTests, stores_single_configuration) {
  adapters::das::serial_number_t sn = "ae73sc";

  EXPECT_FALSE(repo.contains(sn))
      << "Empty repository should not report containtment of "
         "unexisting serial number";

  vnigma::message_variant cmd =
      vnigma::das::set_frequency(1259, device, vnigma::send_period::normal);

  repo.insert(sn, cmd);

  EXPECT_EQ(repo.size(), 1);
  EXPECT_TRUE(repo.contains(sn));
}

TEST_F(DasConfigRepoTests, pops_front_item_like_buffer) {
  adapters::das::serial_number_t sn = "ae73sc";

  vnigma::message_variant cmd =
      vnigma::das::set_frequency(100, device, vnigma::send_period::normal);

  repo.insert(sn, cmd);

  auto buf = repo.next(sn);
  vnigma::buffer expected("<DSASF,100,1,,1000\r\n");

  EXPECT_EQ(buf, expected);

  EXPECT_EQ(repo.size(sn), 0);
}
