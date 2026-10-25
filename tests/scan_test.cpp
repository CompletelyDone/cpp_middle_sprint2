#include <gtest/gtest.h>
#include <print>

#include "scan.hpp"

TEST(ScanTest, SimpleTest) {
    auto result = stdx::scan<uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, std::string, std::string>(
        "24.10.2026 08:35 [AuthService] error", "{%d}.{%d}.{%d} {%d}:{%d} [{%s}] {%s}");
    ASSERT_FALSE(result);
}