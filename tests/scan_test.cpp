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

TEST(ScanTest, SignedInteger) {
    auto result = stdx::scan<int32_t>("100", "{%d}");

    ASSERT_TRUE(result.has_value());

    EXPECT_EQ(std::get<0>(result->values()), 100);
}

TEST(ScanTest, NegativeSignedInteger) {
    auto result = stdx::scan<int32_t>("-100", "{%d}");

    ASSERT_TRUE(result.has_value());

    EXPECT_EQ(std::get<0>(result->values()), -100);
}

TEST(ScanTest, UnsignedInteger) {
    auto result = stdx::scan<uint32_t>("100", "{%u}");

    ASSERT_TRUE(result.has_value());

    EXPECT_EQ(std::get<0>(result->values()), 100);
}

TEST(ScanTest, Float) {
    auto result = stdx::scan<float>("10.1", "{%f}");

    ASSERT_TRUE(result.has_value());

    EXPECT_FLOAT_EQ(std::get<0>(result->values()), 10.1f);
}

TEST(ScanTest, Double) {
    auto result = stdx::scan<double>("10.1", "{%f}");

    ASSERT_TRUE(result.has_value());

    EXPECT_DOUBLE_EQ(std::get<0>(result->values()), 10.1);
}

TEST(ScanTest, String) {
    auto result = stdx::scan<std::string>("Hello, Sniper", "Hello, {%s}");

    ASSERT_TRUE(result.has_value());

    EXPECT_EQ(std::get<0>(result->values()), "Sniper");
}

TEST(ScanTest, StringView) {
    auto result = stdx::scan<std::string_view>("Hello, Yandex", "Hello, {%s}");

    ASSERT_TRUE(result.has_value());

    EXPECT_EQ(std::get<0>(result->values()), "Yandex");
}

TEST(ScanTest, UInt8_tOverflow) {
    auto result = stdx::scan<uint8_t>("256", "{%u}");

    ASSERT_FALSE(result.has_value());

    EXPECT_FALSE(result.error().message.empty());
}

TEST(ScanTest, EmptyPlaceholder) {
    auto result = stdx::scan<uint32_t>("100", "{}");

    ASSERT_TRUE(result.has_value());

    EXPECT_EQ(std::get<0>(result->values()), 100);
}

TEST(ScanTest, ConstSignedInteger) {
    auto result = stdx::scan<const int32_t>("100", "{%d}");

    ASSERT_TRUE(result.has_value());

    EXPECT_EQ(std::get<0>(result->values()), 100);

    using TupleType = std::remove_cvref_t<decltype(result->values())>;
    using ValueType = std::tuple_element_t<0, TupleType>;

    static_assert(std::is_const_v<ValueType>);
}

TEST(ScanTest, VolatileSignedInteger) {
    auto result = stdx::scan<volatile int32_t>("100", "{%d}");

    ASSERT_TRUE(result.has_value());

    EXPECT_EQ(std::get<0>(result->values()), 100);

    using TupleType = std::remove_cvref_t<decltype(result->values())>;
    using ValueType = std::tuple_element_t<0, TupleType>;

    static_assert(std::is_volatile_v<ValueType>);
}

TEST(ScanTest, ErrorInTheMiddle) {
    auto result = stdx::scan<int32_t, double, std::string>("654321 invalid Hello", "{%d} {%f} {%s}");

    ASSERT_FALSE(result.has_value());
    EXPECT_FALSE(result.error().message.empty());
}

TEST(ScanTest, RefSignedInteger) {
    auto result = stdx::scan<int &>("100", "{%d}");

    ASSERT_FALSE(result.has_value());
    EXPECT_FALSE(result.error().message.empty());
}