#pragma once

#include <glad/glad.h>
#include <string>

class Texture
{
public:
    unsigned int id;
    int width;
    int height;
    int channels;

    // 构造时从文件加载
    // flipVertical 表示是否上下翻转（大多数 2D 贴图都要翻一下）
    Texture(const std::string &path, bool flipVertical = true);

    // 绑定到某个纹理单元（默认 0）
    void bind(int unit = 0) const;

    // 释放
    ~Texture();
};