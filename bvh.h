#ifndef BVH_H
#define BVH_H

#include "rtweekend.h"
#include "hittable.h"
#include "hittable_list.h"
#include <algorithm> 
#include <vector>

using std::make_unique;
using std::unique_ptr;

class bvh_node : public hittable
{
public:
    // 构造函数：接收一个 list，构建这棵树
    // 注意：这里必须把 list 的所有权转移进来 (hittable_list 此时会被掏空)
    bvh_node(hittable_list list)
        : bvh_node(list.objects, 0, list.objects.size()) {}

    // 核心构造逻辑：递归构建
    // src_objects: 源物体列表
    // start, end: 当前处理的范围 [start, end)
    bvh_node(std::vector<unique_ptr<hittable>> &src_objects, size_t start, size_t end)
    {

        // 1. 随机选择一个轴 (x=0, y=1, z=2) 来切分
        int axis = int(random_double(0, 3)) % 3;

        // 2. 定义一个比较规则 lambda
        auto comparator = [axis](const unique_ptr<hittable> &a, const unique_ptr<hittable> &b)
        {
            return a->bounding_box().axis(axis).min < b->bounding_box().axis(axis).min;
        };

        size_t object_span = end - start;

        if (object_span == 1)
        {
            // 递归基准 1：只剩一个物体了
            // left 拿走这个物体，right 设为空
            left = std::move(src_objects[start]);
            right = nullptr;
        }
        else if (object_span == 2)
        {
            // 递归基准 2：剩两个物体
            // 比较一下它俩的位置，小的给左边，大的给右边
            if (comparator(src_objects[start], src_objects[start + 1]))
            {
                left = std::move(src_objects[start]);
                right = std::move(src_objects[start + 1]);
            }
            else
            {
                left = std::move(src_objects[start + 1]);
                right = std::move(src_objects[start]);
            }
        }
        else
        {
            // 还有很多物体：先排序，再对半劈开
            std::sort(src_objects.begin() + start, src_objects.begin() + end, comparator);

            size_t mid = start + object_span / 2;

            // 递归构建左子树和右子树
            left = make_unique<bvh_node>(src_objects, start, mid);
            right = make_unique<bvh_node>(src_objects, mid, end);
        }

        // 3. 计算当前节点的包围盒
        // 它是左儿子和右儿子的合体
        if (right == nullptr)
        {
            bbox = left->bounding_box();
        }
        else
        {
            bbox = aabb(left->bounding_box(), right->bounding_box());
        }
    }

    // 核心算法：hit 函数
    bool hit(const ray &r, interval ray_t, hit_record &rec) const override
    {
        // 第一步：如果连大盒子都没撞到，直接返回 false
        // 这一行代码节省了 90% 的计算量！
        if (!bbox.hit(r, ray_t))
            return false;

        // 如果撞到了盒子，就进去问问左儿子撞没撞到
        bool hit_left = left->hit(r, ray_t, rec);

        // 如果左儿子撞到了，由于我们要找最近的交点，
        // 所以右儿子的查找范围最大值 (t_max) 应该缩小到左儿子的交点 (rec.t)
        // 这样如果右儿子比左儿子远，就不用看了
        interval right_t_interval(ray_t.min, hit_left ? rec.t : ray_t.max);

        // 问问右儿子 (注意检查右儿子是不是 nullptr)
        bool hit_right = (right != nullptr) && right->hit(r, right_t_interval, rec);

        return hit_left || hit_right;
    }

    aabb bounding_box() const override { return bbox; }

private:
    unique_ptr<hittable> left;
    unique_ptr<hittable> right;
    aabb bbox;
};

#endif