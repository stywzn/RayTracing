#ifndef AABB_H
#define AABB_H

#include "rtweekend.h"

class aabb
{
public:
    interval x, y, z;

    constexpr aabb() noexcept {}

    constexpr aabb(const interval &ix, const interval &iy, const interval &iz) noexcept
        : x(ix), y(iy), z(iz) {}

    aabb(const point3 &a, const point3 &b) noexcept
    {
        x = interval(std::fmin(a[0], b[0]), std::fmax(a[0], b[0]));
        y = interval(std::fmin(a[1], b[1]), std::fmax(a[1], b[1]));
        z = interval(std::fmin(a[2], b[2]), std::fmax(a[2], b[2]));
    }

    aabb(const aabb &box0, const aabb &box1) noexcept
    {
        x = interval(box0.x, box1.x);
        y = interval(box0.y, box1.y);
        z = interval(box0.z, box1.z);
    }

    const interval &axis(int n) const noexcept
    {
        if (n == 1)
            return y;
        if (n == 2)
            return z;
        return x;
    }

    bool hit(const ray &r, interval ray_t) const noexcept
    {
        for (int a = 0; a < 3; a++)
        {
            auto invD = 1 / r.direction()[a];
            auto orig = r.origin()[a];

            auto t0 = (axis(a).min - orig) * invD;
            auto t1 = (axis(a).max - orig) * invD;

            if (invD < 0)
                std::swap(t0, t1);

            if (t0 > ray_t.min)
                ray_t.min = t0;
            if (t1 < ray_t.max)
                ray_t.max = t1;

            if (ray_t.max <= ray_t.min)
                return false;
        }
        return true;
    }
};

#endif