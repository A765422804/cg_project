#pragma once

#include "Light.h"

class PointLight : public Light
{
public:
    glm::vec3 position; // 点光源的位置
    float constant;     // 衰减常数
    float linear;       // 线性衰减
    float quadratic;    // 二次衰减

    PointLight(glm::vec3 _position, glm::vec3 _color, float _constant = 1.0f, float _linear = 0.09f, float _quadratic = 0.032f)
        : Light(_color), position(_position), constant(_constant), linear(_linear), quadratic(_quadratic) {}

    // 重写 applyLight()，将点光源的参数传递给着色器
    void applyLight(Shader &shader, const std::string &lightName) const override
    {
        shader.setVec3(lightName + ".position", position);
        shader.setVec3(lightName + ".color", color);
        shader.setFloat(lightName + ".constant", constant);
        shader.setFloat(lightName + ".linear", linear);
        shader.setFloat(lightName + ".quadratic", quadratic);
    }
};
