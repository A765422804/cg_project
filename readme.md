# 计算机图形学实验作业

## 如何运行

项目使用 CMake 进行管理，需确保本地安装的 [CMake](https://cmake.org/download/) 版本 ≥ 3.10

### windows

使用 [minGW](https://github.com/niXman/mingw-builds-binaries/releases) 进行编译，步骤如下
1. 确保已将 MinGW 的 bin 目录添加到系统环境变量 Path 中（避免编译时找不到工具链）。
2. 打开PowerShell，执行以下命令启动构建：

```bash
cd cg_project
.\build_windows.bat
```

### macOS

使用 [Homebrew](https://brew.sh/) 安装依赖 glfw：

```bash
brew install glfw
```

打开终端，执行以下命令启动构建：
```bash
cd cg_project
# 先给脚本加执行权限（只需要做一次）
chmod +x build_macos.sh
./build_macos.sh
```

## 项目结构

```
cg_project
├─ .DS_Store
├─ CMakeLists.txt
├─ build_macos.sh
├─ build_windows.bat
├─ image
│  ├─ cornell_box.png
│  └─ debug_mode.png
├─ include
│  ├─ GLFW
│  ├─ KHR
│  ├─ glad
│  ├─ glm
│  └─ stb_image.h
├─ lib
│  └─ libglfw3.a
├─ readme.md
├─ shader
│  ├─ default_fragment_shader.fs
│  ├─ default_vertex_shader.vs
│  ├─ normal_fragment_shader.fs
│  ├─ normal_vertex_shader.vs
│  ├─ phone_fragment_shader.fs
│  ├─ phone_vertex_shader.vs
│  ├─ point_shadow_fragment_shader.fs
│  ├─ point_shadow_vertex_shader.vs
│  ├─ shadow_fragment_shader.fs
│  └─ shadow_vertex_shader.vs
├─ src
│  ├─ Camera.cpp
│  ├─ Camera.h
│  ├─ Shader.cpp
│  ├─ Shader.h
│  ├─ glad.c
│  ├─ light
│  │  ├─ DirectionalLight.h
│  │  ├─ Light.h
│  │  └─ PointLight.h
│  ├─ main.cpp
│  ├─ material
│  │  ├─ Material.h
│  │  ├─ PhoneMaterial.cpp
│  │  ├─ PhoneMaterial.h
│  │  ├─ PureColorMaterial.cpp
│  │  ├─ PureColorMaterial.h
│  │  ├─ TexturedPhoneMaterial.cpp
│  │  └─ TexturedPhoneMaterial.h
│  ├─ object
│  │  ├─ Cone.cpp
│  │  ├─ Cone.h
│  │  ├─ Cube.cpp
│  │  ├─ Cube.h
│  │  ├─ Cylinder.cpp
│  │  ├─ Cylinder.h
│  │  ├─ Object.h
│  │  ├─ Plane.cpp
│  │  ├─ Plane.h
│  │  ├─ Sphere.cpp
│  │  └─ Sphere.h
│  ├─ stb_image_impl.cpp
│  ├─ texture
│  │  ├─ Texture.cpp
│  │  └─ Texture.h
│  └─ tool
│     ├─ Line.cpp
│     ├─ Line.h
│     ├─ Point.cpp
│     └─ Point.h
├─ texture
│  ├─ brick.jpeg
│  ├─ earth.jpg
│  ├─ floor.jpeg
│  ├─ painting.jpg
│  ├─ wall.jpg
│  └─ wood.jpg
├─ 实验报告.md
├─ 实验报告.pdf
├─ 效果演示-点光源 only.mp4
└─ 效果演示.mp4

```