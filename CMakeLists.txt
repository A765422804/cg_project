cmake_minimum_required(VERSION 3.10)   # 版本号
project(cg_project)                    # 项目名

# C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 头文件目录
include_directories(include)

# ===================== 平台相关设置 =====================

if (WIN32)
    # Windows：使用 MinGW 工具链
    # 要求用 MinGW Makefiles 生成器
    if (NOT CMAKE_GENERATOR STREQUAL "MinGW Makefiles")
        message(FATAL_ERROR "请在 Windows 下使用 MinGW Makefiles 生成器，命令示例：cmake .. -G \"MinGW Makefiles\"")
    endif()

    # 使用项目自带的静态 glfw 库
    set(GLFW_LIB "${PROJECT_SOURCE_DIR}/lib/libglfw3.a")

elseif (APPLE)
    # ===================== macOS 下的设置 =====================

    # 找 OpenGL
    find_package(OpenGL REQUIRED)
    find_package(glfw3 QUIET)

else()
    # 直接报错
    message(FATAL_ERROR "当前仅支持 Windows 和 macOS 平台，其他平台请自行配置 CMakeLists.txt")
endif()

# ===================== 源文件收集 =====================

# 包含 src 目录下的所有源文件（包含子目录）
aux_source_directory(src DIR_ALL_SRC)

# 递归地包含子目录中的源文件
aux_source_directory(src/light    DIR_LIGHT_SRC)
aux_source_directory(src/material DIR_MATERIAL_SRC)
aux_source_directory(src/object   DIR_OBJECT_SRC)
aux_source_directory(src/tool     DIR_TOOL_SRC)
aux_source_directory(src/texture  DIR_TEXTURE_SRC)

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
    # Windows + MinGW：glfw 静态库 + Win32 OpenGL 相关库
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
    message(FATAL_ERROR "当前仅支持 Windows 和 macOS 平台，其他平台请自行配置 CMakeLists.txt")
endif()
