#pragma once

#include "Material.h"
#include <glm/glm.hpp>

class PhongMaterial : public Material
{
public:
    glm::vec3 diffuse;
    glm::vec3 specular;
    glm::vec3 ambient;
    float shininess;

    PhongMaterial(const glm::vec3 &diffuse,
                  const glm::vec3 &specular,
                  const glm::vec3 &ambient,
                  float shininess);

    void applyMaterial(Shader &shader) const override;
};