#pragma once

#include "TciProtocol.h"

#include <algorithm>
#include <charconv>
#include <cmath>
#include <optional>
#include <spdlog>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>

namespace ee::tci {

template <typename... Args>
[[maybe_unused]] static inline std::string concatenate(Args const&... args) noexcept
{
    const size_t len = (0 + ... + std::string_view(args).size());
    std::string result;
    result.reserve(len);
    (result.append(args), ...);
    return result;
}

namespace {

    template <typename T>
    std::optional<T> to_num(std::string_view str)
    {
        T result;
        auto [p, ec] = std::from_chars(str.data(), str.data() + str.size(), result);
        if (ec == std::errc())
            return result;
        return std::nullopt;
    }

    template <>
    std::optional<double> to_num(std::string_view str)
    {
        double result;
        bool minus { false };

        if (str.at(0) == '-') {
            minus = true;
        }

        uint64_t pointNumber = str.find_first_of('.');
        if (pointNumber == str.npos) {
            return to_num<int>(str);
        }

        std::string_view pointString;
        if (!minus) {
            pointString = str.substr(0, pointNumber);
        } else {
            pointString = str.substr(1, pointNumber);
        }

        std::from_chars_result fromCharsResult = std::from_chars(pointString.data(), pointString.data() + pointString.size(), pointNumber);
        if (fromCharsResult.ec != std::errc()) {
            return std::nullopt;
        }

        result = pointNumber;

        pointString = str.substr(pointString.size() + 1);
        fromCharsResult = std::from_chars(pointString.data(), pointString.data() + pointString.size(), pointNumber);
        if (fromCharsResult.ec != std::errc()) {
            return std::nullopt;
        }

        uint64_t digitCapacity = std::pow(10, pointString.size());
        result += static_cast<double>(pointNumber) / digitCapacity;

        return minus ? -result : result;
    }

    template <>
    std::optional<float> to_num(std::string_view str)
    {
        float result;
        bool minus { false };

        if (str.at(0) == '-') {
            minus = true;
        }

        uint64_t pointNumber = str.find_first_of('.');
        if (pointNumber == str.npos) {
            return to_num<int>(str);
        }

        std::string_view pointString;
        if (!minus) {
            pointString = str.substr(0, pointNumber);
        } else {
            pointString = str.substr(1, pointNumber);
        }

        std::from_chars_result fromCharsResult = std::from_chars(pointString.data(), pointString.data() + pointString.size(), pointNumber);
        if (fromCharsResult.ec != std::errc()) {
            return std::nullopt;
        }

        result = pointNumber;

        pointString = str.substr(pointString.size() + 1);
        fromCharsResult = std::from_chars(pointString.data(), pointString.data() + pointString.size(), pointNumber);
        if (fromCharsResult.ec != std::errc()) {
            return std::nullopt;
        }

        uint64_t digitCapacity = std::pow(10, pointString.size());
        result += static_cast<float>(pointNumber) / digitCapacity;

        return minus ? -result : result;
    }

    template <typename T>
    inline std::optional<T> from_str(std::string_view arg)
    {
        if (arg.empty())
            return std::nullopt;

        if constexpr (std::is_same_v<T, bool>) {
            std::string t_str { arg };
            return ((t_str.compare("true") == 0) || (t_str.compare("1") == 0));
        } else if constexpr (std::is_same_v<T, std::string>) {
            std::string t_str { arg };
            return t_str;
        } else {
            return to_num<T>(arg);
        }
    }

    template <typename... Args, std::size_t N, std::size_t... I>
    constexpr auto extract_imp(const std::array<std::string_view, N>& src, std::index_sequence<I...>)
    {
        return std::make_tuple((from_str<Args>(src[I]))...);
    }

    template <std::size_t N>
    constexpr std::array<std::string_view, N> static_split(std::string_view src, const char delimeter)
    {
        std::size_t index { 0 };
        std::array<std::string_view, N> t_array;
        for (auto t_begin = src.begin(); t_begin < src.end();) {
            if (const auto t_end = std::find(t_begin, src.end(), delimeter); t_end < src.end()) {
                if (t_begin < t_end)
                    t_array[index++] = src.substr(std::distance(src.begin(), t_begin), std::distance(t_begin, t_end));
                t_begin = t_end + 1;
            } else {
                if (t_begin < t_end)
                    t_array[index++] = src.substr(std::distance(src.begin(), t_begin), std::distance(t_begin, t_end));
                break;
            }

            if (index >= N)
                break;
        }

        return t_array;
    }

    [[maybe_unused]] inline std::size_t arguments(std::string_view src, const char delimeter)
    {
        std::size_t t_args { 0 };
        for (auto t_begin = src.begin(); t_begin < src.end();) {
            if (const auto t_end = std::find(t_begin, src.end(), delimeter); t_end < src.end()) {
                if (t_begin < t_end)
                    ++t_args;
                t_begin = t_end + 1;
            } else {
                if (t_begin < t_end)
                    ++t_args;
                break;
            }
        }
        return t_args;
    }

    template <typename... Args, typename Indices = std::make_index_sequence<sizeof...(Args)>>
    constexpr auto extract(std::string_view src)
    {
        return extract_imp<Args...>(static_split<sizeof...(Args)>(src, ','), Indices {});
    }

    template <typename T>
    static inline std::string stringify(T value)
    {
        if constexpr (std::is_floating_point_v<T>)
            return fmt::format("{:0.1f}", value);
        else
            return fmt::format("{}", value);
    }

    template <typename T, typename... Args>
    static inline std::string stringify(const T& first, Args... args)
    {
        return concatenate(stringify(first), split_args_v, stringify(args...));
    }

}

template <typename... Args>
inline auto deserialize(std::string_view one_command)
{
    const auto ret = static_split<2>(one_command, split_cmd);
    std::string t_cmd { ret[0] };
    return std::tuple<Command, std::tuple<std::optional<Args>...>>(TciMapStr.at(std::string(t_cmd)), extract<Args...>(ret[1]));
}

static inline Command get_cmd(std::string_view one_command)
{
    const auto ret = static_split<2>(one_command, split_cmd);
    return TciMapStr.at(std::string(ret[0]));
}

template <typename... Args>
static inline std::string serialize(Command cmd, Args... args)
{
    return concatenate(TciMapCmd.at(cmd), split_cmd_v, stringify(args...), end_cmd_v);
}

template <>
[[maybe_unused]] std::string serialize(Command cmd)
{
    return concatenate(TciMapCmd.at(cmd), end_cmd_v);
}

template <typename... Args>
static inline std::string serialize_witout_end(Command cmd, Args... args)
{
    return concatenate(TciMapCmd.at(cmd), split_cmd_v, stringify(args...));
}

inline static std::vector<std::string_view> split(std::string_view src, const char delimeter)
{
    std::vector<std::string_view> t_vector;
    for (auto t_begin = src.begin(); t_begin < src.end();) {
        if (const auto t_end = std::find(t_begin, src.end(), delimeter); t_end < src.end()) {
            if (t_begin < t_end)
                t_vector.emplace_back(src.substr(std::distance(src.begin(), t_begin), std::distance(t_begin, t_end)));
            t_begin = t_end + 1;
        } else {
            if (t_begin < t_end)
                t_vector.emplace_back(src.substr(std::distance(src.begin(), t_begin), std::distance(t_begin, t_end)));
            break;
        }
    }
    return t_vector;
}

} // namespace ee::tci
