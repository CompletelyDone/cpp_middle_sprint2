#pragma once

#include "parse.hpp"
#include "types.hpp"
#include <cstddef>
#include <expected>
#include <tuple>
#include <type_traits>
#include <utility>

namespace stdx {

template <std::size_t Index, typename Result, typename First, typename... Next, typename... Values>
[[nodiscard]] auto process_recurse(const std::pair<std::vector<std::string_view>, std::vector<std::string_view>> &pair,
                                   Values... values) -> std::expected<Result, details::scan_error> {
    auto result = details::parse_value_with_format<First>(pair.second[Index], pair.first[Index]);
    if (!result) {
        return std::unexpected(result.error());
    }

    if constexpr (sizeof...(Next) > 0) {
        return process_recurse<Index + 1, Result, Next...>(pair, values..., result.value());
    } else {
        return Result{std::tuple<Values..., First>{values..., result.value()}};
    }
}

template <typename... Ts>
[[nodiscard]] std::expected<details::scan_result<Ts...>, details::scan_error> scan(std::string_view input,
                                                                                   std::string_view format) {
    if constexpr ((std::is_reference_v<Ts> || ...)) {
        return std::unexpected(details::scan_error("Reference types are not supported. "));
    } else {
        auto parsed = details::parse_sources(input, format);
        if (!parsed.has_value())
            return std::unexpected(parsed.error());

        auto vec_pair = parsed.value();
        if (vec_pair.first.size() != vec_pair.second.size() || vec_pair.first.size() != sizeof...(Ts)) {
            return std::unexpected(details::scan_error("Size mismatched. "));
        }

        using Result = details::scan_result<Ts...>;
        return process_recurse<0, Result, Ts...>(vec_pair);
    }
}

}  // namespace stdx
