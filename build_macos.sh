# 如果已经有 build 目录就删掉
if [ -d build ]; then
    rm -rf build
fi

# 重新创建 build 并进入
mkdir build
cd build

# 配置 & 编译
cmake ..
make -j4

# 运行可执行文件
./cg_project