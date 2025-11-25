#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "vec3.h"
#include <memory>

class sphere : public hittable
{
public:
    sphere(const point3 &center, double radius, std::unique_ptr<material> mat)
        : center(center), radius(std::fmax(0, radius)), mat(std::move(mat))
    {
        // ✅ 新增逻辑：在构造时计算包围盒
        // 这里的原理很简单：球心减半径，球心加半径，这就定出了一个正方体盒子
        auto rvec = vec3(radius, radius, radius);
        bbox = aabb(center - rvec, center + rvec);
    }

    bool hit(const ray &r, interval ray_t, hit_record &rec) const override
    {
        vec3 oc = center - r.origin();
        auto a = r.direction().length_squared();
        auto h = dot(r.direction(), oc);
        auto c = oc.length_squared() - radius * radius;

        auto discriminant = h * h - a * c;
        if (discriminant < 0)
            return false;

        auto sqrtd = std::sqrt(discriminant);

        // Find the nearest root that lies in the acceptable range.
        auto root = (h - sqrtd) / a;
        if (!ray_t.surrounds(root))
        {
            root = (h + sqrtd) / a;
            if (!ray_t.surrounds(root))
                return false;
        }

        rec.t = root;
        rec.p = r.at(rec.t);
        vec3 outward_normal = (rec.p - center) / radius;
        rec.set_face_normal(r, outward_normal);

        rec.mat = mat.get();

        return true;
    }

    // ✅ 新增接口实现：返回包围盒
    // 必须实现这个函数，否则 sphere 就是抽象类，无法实例化
    aabb bounding_box() const override { return bbox; }

private:
    point3 center;
    double radius;
    std::unique_ptr<material> mat;

    // ✅ 新增成员变量：存储这个球的包围盒
    aabb bbox;
};
#endif