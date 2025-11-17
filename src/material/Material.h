#pragma once

#include "../Shader.h"

class Material
{
public:
    virtual ~Material() {}

    // 把自己对应的 uniform 全部设置到 shader 里
    virtual void applyMaterial(Shader &shader) const = 0;
};