#ifndef INTERVAL_H
#define INTERVAL_H

#include "rtweekend.h" // 确保能用到 infinity

class interval
{
public:
    double min, max;

    // ✅ 优化 1: 默认构造函数 (constexpr + noexcept)
    constexpr interval() noexcept : min(+infinity), max(-infinity) {}

    // ✅ 优化 2: 标准构造函数 (constexpr + noexcept)
    constexpr interval(double min, double max) noexcept : min(min), max(max) {}

    // ✅ 关键新增: 合并两个区间的构造函数
    // 用于 AABB 计算两个盒子的并集
    // 注意: fmin/fmax 在 C++17 不是 constexpr，所以这里只加 noexcept
    interval(const interval &a, const interval &b) noexcept
        : min(std::fmin(a.min, b.min)), max(std::fmax(a.max, b.max)) {}

    constexpr double size() const noexcept
    {
        return max - min;
    }

    constexpr bool contains(double x) const noexcept
    {
        return min <= x && x < +max;
    }

    constexpr bool surrounds(double x) const noexcept
    {
        return min < x && x < max;
    }

    constexpr double clamp(double x) const noexcept
    {
        if (x < min)
            return min;
        if (x > max)
            return max;
        return x;
    }

    static const interval empty, universe;
};

// ✅ 优化 3: 加上 inline 防止多重定义错误 (C++17)
inline const interval interval::empty = interval(+infinity, -infinity);
inline const interval interval::universe = interval(-infinity, +infinity);

#endif