#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "ags/vst3-capi/host/hostcontext.h"

class HostMessageSecurityTest : public ::testing::TestWithParam<std::string> {};

TEST_P(HostMessageSecurityTest, SetMessageID_NeverExceedsAllocatedBuffer) {
    // Invariant: Buffer reads never exceed the declared length
    std::string payload = GetParam();
    
    // Create HostMessage instance
    HostMessage msg;
    
    // This should not cause heap buffer overflow
    msg.setMessageID(payload.c_str());
    
    // Verify the message ID was set correctly or handled safely
    const char* storedId = msg.getMessageID();
    if (payload.empty()) {
        EXPECT_EQ(storedId, nullptr);
    } else {
        ASSERT_NE(storedId, nullptr);
        // Ensure null termination exists within bounds
        EXPECT_EQ(storedId[payload.size()], '\0');
    }
}

INSTANTIATE_TEST_SUITE_P(
    AdversarialInputs,
    HostMessageSecurityTest,
    ::testing::Values(
        // Valid input (boundary case)
        "normal_id",
        // Exact exploit case - exceeds typical buffer expectations
        std::string(1024, 'A'),  // 1KB payload
        // Extreme oversized input
        std::string(10000, 'B'), // 10KB payload
        // Empty string edge case
        ""
    )
);

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}