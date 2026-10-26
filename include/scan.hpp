#pragma once

#include "parse.hpp"
#include "types.hpp"
#include <cstddef>
#include <expected>
#include <optional>
#include <tuple>
#include <utility>

namespace stdx {

template <typename Result, typename First, typename... Next, typename... Values>
auto process_recurse(std::size_t index,
                     const std::pair<std::vector<std::string_view>, std::vector<std::string_view>> &pair,
                     Values... values) -> std::expected<Result, details::scan_error> {
    auto result = details::parse_value_with_format<First>(pair.second[index], pair.first[index]);
    if (!result) {
        return std::unexpected(result.error());
    }

    if constexpr (sizeof...(Next) > 0) {
        return process_recurse<Result, Next...>(index + 1, pair, values..., result.value());
    } else {
        return Result{std::tuple<Values..., First>{values..., result.value()}};
    }
}

template <typename... Ts>
std::expected<details::scan_result<Ts...>, details::scan_error> scan(std::string_view input, std::string_view format) {
    auto parsed = details::parse_sources(input, format);
    if (!parsed.has_value())
        return std::unexpected(parsed.error());

    auto vec_pair = parsed.value();
    if (vec_pair.first.size() != vec_pair.second.size() || vec_pair.first.size() != sizeof...(Ts)) {
        return std::unexpected(details::scan_error("Size mismatched. "));
    }

    using Result = details::scan_result<Ts...>;
    return process_recurse<Result, Ts...>(0, vec_pair);
}

}  // namespace stdx
