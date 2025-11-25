#ifndef HITTABLE_H
#define HITTABLE_H

#include "ray.h"
#include "aabb.h" // ✅ 1. 必须引入这个，因为我们要返回 aabb 对象

class material;

class hit_record
{
public:
    point3 p;
    vec3 normal;
    material *mat;
    double t;
    bool front_face;

    void set_face_normal(const ray &r, const vec3 &outward_normal)
    {
        // Set the hit record normal vector.
        // NOTE: the parameter 'outward_normal' is assumed to have unit length.

        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class hittable
{
public:
    virtual ~hittable() = default;

    virtual bool hit(const ray &r, interval ray_t, hit_record &rec) const = 0;

    // ✅ 2. 新增接口：获取包围盒
    // 这是一个纯虚函数 (= 0)，意味着所有继承自 hittable 的类（比如 sphere）
    // 都必须实现这个函数，否则编译报错。
    virtual aabb bounding_box() const = 0;
};

#endif