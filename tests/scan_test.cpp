#include <gtest/gtest.h>

#include "scan.hpp"

TEST(ScanTest, WrongSpecIntUint) {
    auto result = stdx::scan<uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, std::string, std::string>(
        "24.10.2026 08:35 [AuthService] error", "{%d}.{%d}.{%d} {%d}:{%d} [{%s}] {%s}");
    ASSERT_FALSE(result);
    EXPECT_FALSE(result.error().message.empty());
}

TEST(ScanTest, Success) {
    auto result = stdx::scan<uint8_t, uint8_t, uint16_t, uint8_t, uint8_t, std::string, std::string>(
        "24.10.2026 08:35 [AuthService] error", "{%u}.{%u}.{%u} {%u}:{%u} [{%s}] {%s}");
    
    ASSERT_TRUE(result.has_value()) << result.error().message;

    auto values = result->values();

    EXPECT_EQ(std::get<0>(values), 24);
    EXPECT_EQ(std::get<1>(values), 10);
    EXPECT_EQ(std::get<2>(values), 2026);
    EXPECT_EQ(std::get<3>(values), 8);
    EXPECT_EQ(std::get<4>(values), 35);
    EXPECT_EQ(std::get<5>(values), "AuthService");
    EXPECT_EQ(std::get<6>(values), "error");
}

TEST(ScanTest, SignedInteger)
{
    auto result = stdx::scan<int32_t>("100", "{%d}");

    ASSERT_TRUE(result.has_value());

    EXPECT_EQ(std::get<0>(result->values()), 100);
}

TEST(ScanTest, NegativeSignedInteger)
{
    auto result = stdx::scan<int32_t>("-100", "{%d}");

    ASSERT_TRUE(result.has_value());

    EXPECT_EQ(std::get<0>(result->values()), -100);
}

TEST(ScanTest, UnsignedInteger)
{
    auto result = stdx::scan<uint32_t>("100", "{%u}");

    ASSERT_TRUE(result.has_value());

    EXPECT_EQ(std::get<0>(result->values()), 100);
}

TEST(ScanTest, Float)
{
    auto result = stdx::scan<float>("10.1", "{%f}");

    ASSERT_TRUE(result.has_value());

    EXPECT_EQ(std::get<0>(result->values()), 10.1f);
}

TEST(ScanTest, Double)
{
    auto result = stdx::scan<double>("10.1", "{%f}");

    ASSERT_TRUE(result.has_value());

    EXPECT_EQ(std::get<0>(result->values()), 10.1d);
}

TEST(ScanTest, String)
{
    auto result = stdx::scan<std::string>("Hello, Sniper", "Hello, {%s}");

    ASSERT_TRUE(result.has_value());

    EXPECT_EQ(std::get<0>(result->values()), "Sniper");
}

TEST(ScanTest, StringView)
{
    auto result = stdx::scan<std::string_view>("Hello, Yandex", "Hello, {%s}");

    ASSERT_TRUE(result.has_value());

    EXPECT_EQ(std::get<0>(result->values()), "Yandex");
}

TEST(ScanTest, UInt8_tOverflow)
{
    auto result = stdx::scan<uint8_t>("Hello, Sniper530", "Hello, Sniper{%u}");

    ASSERT_FALSE(result.has_value());

    EXPECT_FALSE(result.error().message.empty());
}

TEST(ScanTest, EmptyPlaceholder)
{
    auto result = stdx::scan<uint32_t>("100", "{}");

    ASSERT_TRUE(result.has_value());

    EXPECT_EQ(std::get<0>(result->values()), 100);
}