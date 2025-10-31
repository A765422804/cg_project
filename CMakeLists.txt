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

# 4. （可选）设置 C 编译 flags（如果需要编译 C 代码）
set(CMAKE_C_FLAGS "--target=x86_64-w64-mingw32 ${CMAKE_C_FLAGS}" CACHE STRING "C 编译选项" FORCE)

cmake_minimum_required(VERSION 4.1)                  #版本号
project(test)                                         #项目名
include_directories(include)                           #把include纳入包含目录中
link_libraries("${PROJECT_SOURCE_DIR}/lib/libglfw3.a") #链接之前生成的静态库文件
aux_source_directory(./src DIR_ALL_SRC)				   #src目录下所有文件取个DIR_ALL_SRC名字
add_executable(test ${DIR_ALL_SRC})                   #生成可执行文件
