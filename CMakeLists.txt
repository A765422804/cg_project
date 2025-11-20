cmake_minimum_required(VERSION 3.10)   # 版本号
project(cg_project)                    # 项目名

# C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 头文件目录
include_directories(include)

# ===================== 平台相关设置 =====================

if (WIN32)
    # 1. 指定生成器（MinGW Makefiles）
    # 注意：生成器通常在命令行通过 -G 指定，CMakeLists.txt 中无法直接设置生成器
    # 解决方案：只在 Windows 下检查生成器；macOS 下不检查，直接用默认 Unix Makefiles
    if (NOT CMAKE_GENERATOR STREQUAL "MinGW Makefiles")
        message(FATAL_ERROR "请在 Windows 下使用 MinGW Makefiles 生成器，命令示例：cmake .. -G \"MinGW Makefiles\"")
    endif()

    # 2. 指定 C/C++ 编译器为 Clang（你的原始设置）
    set(CMAKE_C_COMPILER clang CACHE STRING "C 编译器" FORCE)
    set(CMAKE_CXX_COMPILER clang++ CACHE STRING "C++ 编译器" FORCE)

    # 3. 设置 C/C++ 编译 flags（MinGW 交叉编译目标）
    set(CMAKE_CXX_FLAGS "--target=x86_64-w64-mingw32 ${CMAKE_CXX_FLAGS}" CACHE STRING "C++ 编译选项" FORCE)
    set(CMAKE_C_FLAGS  "--target=x86_64-w64-mingw32 ${CMAKE_C_FLAGS}"  CACHE STRING "C 编译选项"  FORCE)

    # 4. Windows 下使用项目自带的静态 glfw 库
    set(GLFW_LIB "${PROJECT_SOURCE_DIR}/lib/libglfw3.a")

elseif (APPLE)
    # ===================== macOS 下的设置 =====================

    # 找 OpenGL
    find_package(OpenGL REQUIRED)
    find_package(glfw3 QUIET)

else()
    # 其他平台（例如 Linux），简单处理
    find_package(OpenGL REQUIRED)
    find_package(glfw3 QUIET)
endif()

# ===================== 源文件收集 =====================

# 包含 src 目录下的所有源文件（包含子目录）
aux_source_directory(src DIR_ALL_SRC)

# 递归地包含子目录中的源文件
aux_source_directory(src/light   DIR_LIGHT_SRC)
aux_source_directory(src/material DIR_MATERIAL_SRC)
aux_source_directory(src/object  DIR_OBJECT_SRC)
aux_source_directory(src/tool    DIR_TOOL_SRC)
aux_source_directory(src/texture DIR_TEXTURE_SRC)

# 将所有子目录的源文件合并到一起
list(APPEND DIR_ALL_SRC
    ${DIR_LIGHT_SRC}
    ${DIR_MATERIAL_SRC}
    ${DIR_OBJECT_SRC}
    ${DIR_TOOL_SRC}
    ${DIR_TEXTURE_SRC}
)

# 创建可执行文件
add_executable(cg_project ${DIR_ALL_SRC})

# ===================== 链接库 =====================

if (WIN32)
    # Windows：静态 glfw + OpenGL + 常见系统库
    target_link_libraries(cg_project PRIVATE
        ${GLFW_LIB}
        opengl32
        gdi32
        user32
        shell32
    )

elseif (APPLE)
    # macOS：OpenGL + glfw + 必要系统框架（Cocoa / IOKit / CoreVideo）

    if (TARGET glfw)
        # 如果 find_package(glfw3) 提供了 glfw 目标
        target_link_libraries(cg_project PRIVATE glfw)
    elseif (glfw3_FOUND)
        # 某些配置下目标名字是 glfw
        target_link_libraries(cg_project PRIVATE glfw)
    elseif (GLFW_LIB)
        # 退而求其次，直接链接找到的库文件
        target_link_libraries(cg_project PRIVATE ${GLFW_LIB})
    else()
        message(FATAL_ERROR "未找到 glfw，请在 macOS 下通过 Homebrew 安装：brew install glfw")
    endif()

    target_link_libraries(cg_project PRIVATE
        OpenGL::GL
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
    )

else()
    # 其他平台（如 Linux）：尝试链接 glfw + OpenGL
    if (TARGET glfw)
        target_link_libraries(cg_project PRIVATE glfw OpenGL::GL)
    elseif (glfw3_FOUND)
        target_link_libraries(cg_project PRIVATE glfw OpenGL::GL)
    else()
        message(WARNING "未找到 glfw3，请在当前平台自行配置链接库")
    endif()
endif()