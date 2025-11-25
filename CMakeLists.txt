cmake_minimum_required(VERSION 3.10)

# 项目名称
project(RayTracer VERSION 1.0)

# 设置 C++ 标准为 17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# ✅ 关键：设置优化选项 (对应你之前的 g++ 参数)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-O3 -march=native -pthread)
endif()

# 查找所有的 .h 和 .cpp 文件 (如果是头文件都在当前目录，这样写就行)
file(GLOB SOURCES "*.cpp" "*.h")

# 生成可执行文件
add_executable(my_raytracer main.cpp)

# ✅ 关键：链接线程库 (对应 -pthread)
find_package(Threads REQUIRED)
target_link_libraries(my_raytracer Threads::Threads)
