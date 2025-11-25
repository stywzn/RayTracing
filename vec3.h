#ifndef VEC3_H
#define VEC3_H

#include <cmath>
#include <iostream>

double random_double();
double random_double(double min, double max);

class vec3
{
public:
    double e[3];

    constexpr vec3() noexcept : e{0, 0, 0} {}
    constexpr vec3(double e0, double e1, double e2) noexcept : e{e0, e1, e2} {}

    constexpr double x() const noexcept { return e[0]; }
    constexpr double y() const noexcept { return e[1]; }
    constexpr double z() const noexcept { return e[2]; }

    constexpr vec3 operator-() const noexcept { return vec3(-e[0], -e[1], -e[2]); }
    constexpr double operator[](int i) const noexcept { return e[i]; }
    constexpr double &operator[](int i) noexcept { return e[i]; }

    vec3 &operator*=(double t) noexcept
    {
        e[0] *= t;
        e[1] *= t;
        e[2] *= t;
        return *this;
    }

    vec3 &operator/=(double t) noexcept
    {
        return *this *= 1 / t;
    }

    vec3 &operator+=(const vec3 &v) noexcept
    {
        e[0] += v.e[0];
        e[1] += v.e[1];
        e[2] += v.e[2];
        return *this;
    }

    // length 用到了 sqrt，sqrt 在 C++17 不是 constexpr，所以只能加 noexcept
    double length() const noexcept
    {
        return std::sqrt(length_squared());
    }

    // length_squared 是纯数学运算，可以 constexpr
    constexpr double length_squared() const noexcept
    {
        return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
    }

    bool near_zero() const noexcept
    {
        // Return true if the vector is close to zero in all dimensions.
        auto s = 1e-8;
        return (std::fabs(e[0]) < s) && (std::fabs(e[1]) < s) && (std::fabs(e[2]) < s);
    }

    static vec3 random()
    {
        return vec3(random_double(), random_double(), random_double());
    }

    static vec3 random(double min, double max)
    {
        return vec3(random_double(min, max), random_double(min, max), random_double(min, max));
    }
};

// point3 is just an alias for vec3
using point3 = vec3;

// Vector Utility Functions

// IO 操作不能 constexpr
inline std::ostream &operator<<(std::ostream &out, const vec3 &v)
{
    return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

// ✅ 优化 4: 所有的简单数学运算都加上 constexpr noexcept
inline constexpr vec3 operator+(const vec3 &u, const vec3 &v) noexcept
{
    return vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}

inline constexpr vec3 operator-(const vec3 &u, const vec3 &v) noexcept
{
    return vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

inline constexpr vec3 operator*(const vec3 &u, const vec3 &v) noexcept
{
    return vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}

inline constexpr vec3 operator*(double t, const vec3 &v) noexcept
{
    return vec3(t * v.e[0], t * v.e[1], t * v.e[2]);
}

inline constexpr vec3 operator*(const vec3 &v, double t) noexcept
{
    return t * v;
}

inline constexpr vec3 operator/(const vec3 &v, double t) noexcept
{
    return (1 / t) * v;
}

inline constexpr double dot(const vec3 &u, const vec3 &v) noexcept
{
    return u.e[0] * v.e[0] + u.e[1] * v.e[1] + u.e[2] * v.e[2];
}

inline constexpr vec3 cross(const vec3 &u, const vec3 &v) noexcept
{
    return vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
                u.e[2] * v.e[0] - u.e[0] * v.e[2],
                u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}

// 用了 sqrt，不能 constexpr，但可以 noexcept
inline vec3 unit_vector(const vec3 &v) noexcept
{
    return v / v.length();
}

// 下面这些涉及到随机数，绝对不能 constexpr
inline vec3 random_in_unit_disk()
{
    while (true)
    {
        auto p = vec3(random_double(-1, 1), random_double(-1, 1), 0);
        if (p.length_squared() < 1)
            return p;
    }
}

inline vec3 random_unit_vector()
{
    while (true)
    {
        auto p = vec3::random(-1, 1);
        auto lensq = p.length_squared();
        if (lensq < 1e-160)
            return vec3(1, 0, 0); // 防止除以0
        if (lensq < 1)
            return p / sqrt(lensq);
    }
}

inline vec3 random_on_hemisphere(const vec3 &normal)
{
    vec3 on_unit_sphere = random_unit_vector();
    if (dot(on_unit_sphere, normal) > 0.0)
        return on_unit_sphere;
    else
        return -on_unit_sphere;
}

// reflect 很简单，可以 constexpr
inline constexpr vec3 reflect(const vec3 &v, const vec3 &n) noexcept
{
    return v - 2 * dot(v, n) * n;
}

// refract 用了 sqrt 和 fmin，不能 constexpr
inline vec3 refract(const vec3 &uv, const vec3 &n, double etai_over_etat) noexcept
{
    auto cos_theta = std::fmin(dot(-uv, n), 1.0);
    vec3 r_out_perp = etai_over_etat * (uv + cos_theta * n);
    vec3 r_out_parallel = -std::sqrt(std::fabs(1.0 - r_out_perp.length_squared())) * n;
    return r_out_perp + r_out_parallel;
}

#endif