#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "hittable.h"
#include "aabb.h" // ✅ 1. 引入包围盒头文件
#include <memory>
#include <vector>

using std::make_unique;
using std::unique_ptr;

class hittable_list : public hittable
{
public:
    std::vector<unique_ptr<hittable>> objects;

    // ✅ 2. 新增成员变量：存储整个列表的大包围盒
    aabb bbox;

    hittable_list() {}
    hittable_list(unique_ptr<hittable> object) { add(std::move(object)); }

    void clear()
    {
        objects.clear();
        bbox = aabb(); // 清空时重置包围盒为“空”
    }

    // ✅ 3. 修改 add 函数：每次添加物体，都扩大包围盒
    void add(unique_ptr<hittable> object)
    {
        // 先把新物体的盒子和现有的盒子合并，更新 bbox
        bbox = aabb(bbox, object->bounding_box());

        // 然后再把物体存进去
        objects.push_back(std::move(object));
    }

    // Hit 函数保持不变
    bool hit(const ray &r, interval ray_t, hit_record &rec) const override
    {
        hit_record tmep_rec;
        bool hit_anything = false;
        auto closest_so_far = ray_t.max;

        for (const auto &object : objects)
        {
            if (object->hit(r, interval(ray_t.min, closest_so_far), tmep_rec))
            {
                hit_anything = true;
                closest_so_far = tmep_rec.t;
                rec = tmep_rec;
            }
        }
        return hit_anything;
    }

    // ✅ 4. 实现接口：交出包围盒
    aabb bounding_box() const override { return bbox; }
};

#endif