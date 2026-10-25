#pragma once

#include "parse.hpp"
#include "types.hpp"
#include <cstddef>
#include <expected>
#include <optional>
#include <tuple>
#include <utility>

namespace stdx {

template <typename... Ts>
std::expected<details::scan_result<Ts...>, details::scan_error> scan(std::string_view input, std::string_view format) {
    auto parsed = details::parse_sources(input, format);
    if (!parsed.has_value())
        return std::unexpected(parsed.error());

    auto &[fmts, inputs] = parsed.value();
    if (fmts.size() != inputs.size() || fmts.size() != sizeof...(Ts)) {
        return std::unexpected(details::scan_error("Size mismatched. "));
    }

    return [&]<std::size_t... Index>(
               std::index_sequence<Index...>) -> std::expected<details::scan_result<Ts...>, details::scan_error> {
        auto results = std::tuple{details::parse_value_with_format<std::tuple_element_t<Index, std::tuple<Ts...>>>(
            inputs[Index], fmts[Index])...};

        bool results_good = std::apply([](auto &&...res) { return (res.has_value() && ...); }, results);
        if (!results_good) {
            std::optional<details::scan_error> first_error;
            std::apply(
                [&first_error](auto &&...res) {
                    ((!res.has_value() && !first_error ? (first_error = res.error(), true) : false), ...);
                },
                results);
            return std::unexpected(*first_error);
        }

        auto values = std::apply([](auto &&...es) { return std::tuple<Ts...>{*es...}; }, results);

        return details::scan_result<Ts...>{values};
    }(std::index_sequence_for<Ts...>{});
}

}  // namespace stdx
