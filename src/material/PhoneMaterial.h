#pragma once

#include "Material.h"

class PhongMaterial : public Material
{
public:
    PhongMaterial(glm::vec3 _diffuse, glm::vec3 _specular, glm::vec3 _ambient, float _shininess)
        : Material(_diffuse, _specular, _ambient, _shininess) {}

    void applyMaterial(Shader &shader) const override
    {
        Material::applyMaterial(shader); // 直接调用基类的 applyMaterial
    }
};

