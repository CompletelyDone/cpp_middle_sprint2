#pragma once

#include <string>
#include <tuple>

namespace stdx::details {

// Класс для хранения ошибки неуспешного сканирования

struct scan_error {
    std::string message;
};

// Шаблонный класс для хранения результатов успешного сканирования

template <typename... Ts>
struct scan_result {
private:
    std::tuple<Ts...> m_Result;

public:
    auto values() const { return m_Result; }
};

}  // namespace stdx::details
