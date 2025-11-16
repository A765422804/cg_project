#pragma once

#include <glm/glm.hpp>
#include <string>

#include "../Shader.h"

class Light
{
public:
    // 光源的颜色
    glm::vec3 color;

    // 构造函数
    Light(glm::vec3 _color) : color(_color) {}

    virtual void applyLight(Shader& shader, const std::string& lightName) const = 0;

    virtual ~Light() {}
};
