# 1. 指定生成器（MinGW Makefiles）
# 注意：生成器通常在命令行通过 -G 指定，CMakeLists.txt 中无法直接设置生成器
# 解决方案：在 CMakeLists.txt 中检查生成器，不符合则报错提示
if(NOT CMAKE_GENERATOR STREQUAL "MinGW Makefiles")
    message(FATAL_ERROR "请使用 MinGW Makefiles 生成器，命令：cmake .. -G \"MinGW Makefiles\"")
endif()

# 2. 指定 C/C++ 编译器为 Clang
set(CMAKE_C_COMPILER clang CACHE STRING "C 编译器" FORCE)
set(CMAKE_CXX_COMPILER clang++ CACHE STRING "C++ 编译器" FORCE)

# 3. 设置 C++ 编译 flags（强制 MinGW 兼容模式）
set(CMAKE_CXX_FLAGS "--target=x86_64-w64-mingw32 ${CMAKE_CXX_FLAGS}" CACHE STRING "C++ 编译选项" FORCE)

# 4. 设置 C 编译 flags（如果需要编译 C 代码）
set(CMAKE_C_FLAGS "--target=x86_64-w64-mingw32 ${CMAKE_C_FLAGS}" CACHE STRING "C 编译选项" FORCE)

cmake_minimum_required(VERSION 3.10)                  #版本号
project(cg_project)                                    #项目名
include_directories(include)                           #把include纳入包含目录中
link_libraries("${PROJECT_SOURCE_DIR}/lib/libglfw3.a") # glfw

# 包含 src 目录下的所有源文件（包含子目录）
aux_source_directory(src DIR_ALL_SRC)

# 递归地包含子目录中的源文件
aux_source_directory(src/light DIR_LIGHT_SRC)
aux_source_directory(src/material DIR_MATERIAL_SRC)
aux_source_directory(src/object DIR_OBJECT_SRC)
aux_source_directory(src/tool DIR_TOOL_SRC)
aux_source_directory(src/texture DIR_TEXTURE_SRC)

# 将所有子目录的源文件合并到一起
list(APPEND DIR_ALL_SRC ${DIR_LIGHT_SRC} ${DIR_MATERIAL_SRC} ${DIR_OBJECT_SRC} ${DIR_TOOL_SRC} ${DIR_TEXTURE_SRC})

# 创建可执行文件
add_executable(cg_project ${DIR_ALL_SRC})
