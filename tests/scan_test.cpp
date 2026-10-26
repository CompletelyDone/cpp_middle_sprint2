#include <gtest/gtest.h>
#include <print>

#include "scan.hpp"

TEST(ScanTest, SimpleTest) {
    auto result = stdx::scan<uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, std::string, std::string>(
        "24.10.2026 08:35 [AuthService] error", "{%d}.{%d}.{%d} {%d}:{%d} [{%s}] {%s}");
    ASSERT_FALSE(result);
    EXPECT_FALSE(result.error().message.empty());
}

TEST(ScanTest, SimpleTest1) {
    auto result = stdx::scan<uint8_t, uint8_t, uint16_t, uint8_t, uint8_t, std::string, std::string>(
        "24.10.2026 08:35 [AuthService] error", "{%u}.{%u}.{%u} {%u}:{%u} [{%s}] {%s}");
    ASSERT_TRUE(result.has_value());
}