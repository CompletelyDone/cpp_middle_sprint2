#pragma once

#include <cstdint>
#include <expected>
#include <iostream>
#include <print>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

#include "types.hpp"

namespace stdx::details {

template <typename T, typename... Args>
inline constexpr bool is_one_of = (std::is_same_v<T, Args> || ...);

template <typename T>
concept FormatUCompatible = is_one_of<T, uint8_t, uint16_t, uint32_t, uint64_t>;

template <typename T>
concept FormatDCompatible = is_one_of<T, int8_t, int16_t, int32_t, int64_t>;

template <typename T>
concept FormatFCompatible = is_one_of<T, float, double>;

template <typename T>
concept FormatSCompatible = is_one_of<T, std::string, std::string_view>;

// Функция для парсинга значения с учетом спецификатора формата
template <typename T>
std::expected<T, scan_error> parse_value_with_format(std::string_view input, std::string_view fmt) {
    using RemCV = std::remove_cv_t<T>;

    if constexpr (FormatDCompatible<RemCV>) {
        if (fmt == "" || fmt != "%d") {
            return std::unexpected(scan_error("Conversion specifier error."));
        }
        std::println(std::cout, "Format D");
    } else if constexpr (FormatSCompatible<RemCV>) {
        if (fmt == "" || fmt != "%s") {
            return std::unexpected(scan_error("Conversion specifier error."));
        }
        std::println(std::cout, "Format S");
    } else if constexpr (FormatFCompatible<RemCV>) {
        if (fmt == "" || fmt != "%f") {
            return std::unexpected(scan_error("Conversion specifier error."));
        }
        std::println(std::cout, "Format F");
    } else if constexpr (FormatUCompatible<RemCV>) {
        if (fmt == "" || fmt != "%u") {
            return std::unexpected(scan_error("Conversion specifier error."));
        }
        std::println(std::cout, "Format U");
    } else {
        return std::unexpected(
            scan_error(std::format("Conversion specifier error. Format {} doesn't supported.", fmt)));
    }
}

// Функция для проверки корректности входных данных и выделения из обеих строк интересующих данных для парсинга
template <typename... Ts>
std::expected<std::pair<std::vector<std::string_view>, std::vector<std::string_view>>, scan_error>
parse_sources(std::string_view input, std::string_view format) {
    std::vector<std::string_view> format_parts;  // Части формата между {}
    std::vector<std::string_view> input_parts;
    size_t start = 0;
    while (true) {
        size_t open = format.find('{', start);
        if (open == std::string_view::npos) {
            break;
        }
        size_t close = format.find('}', open);
        if (close == std::string_view::npos) {
            break;
        }

        // Если между предыдущей } и текущей { есть текст,
        // проверяем его наличие во входной строке
        if (open > start) {
            std::string_view between = format.substr(start, open - start);
            auto pos = input.find(between);
            if (input.size() < between.size() || pos == std::string_view::npos) {
                return std::unexpected(scan_error{"Unformatted text in input and format string are different"});
            }
            if (start != 0) {
                input_parts.emplace_back(input.substr(0, pos));
            }

            input = input.substr(pos + between.size());
        }

        // Сохраняем спецификатор формата (то, что между {})
        format_parts.push_back(format.substr(open + 1, close - open - 1));
        start = close + 1;
    }

    // Проверяем оставшийся текст после последней }
    if (start < format.size()) {
        std::string_view remaining_format = format.substr(start);
        auto pos = input.find(remaining_format);
        if (input.size() < remaining_format.size() || pos == std::string_view::npos) {
            return std::unexpected(scan_error{"Unformatted text in input and format string are different"});
        }
        input_parts.emplace_back(input.substr(0, pos));
        input = input.substr(pos + remaining_format.size());
    } else {
        input_parts.emplace_back(input);
    }
    return std::pair{format_parts, input_parts};
}

}  // namespace stdx::details