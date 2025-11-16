#pragma once

#include "Light.h"

class DirectionalLight : public Light
{
public:
    glm::vec3 direction; // 平行光的方向

    DirectionalLight(glm::vec3 _direction, glm::vec3 _color)
        : Light(_color), direction(_direction) {}

    // 重写 applyLight()，将平行光的参数传递给着色器
    void applyLight(Shader &shader, const std::string &lightName) const override
    {
        // 修改为符合新的命名规范：使用 u + 单词大写格式
        shader.setVec3("uLightDirection", direction); // 更新光源方向
        shader.setVec3("uLightColor", color);         // 更新光源颜色
    }
};
