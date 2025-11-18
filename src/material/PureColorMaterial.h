#pragma once

#include "Material.h"
#include <glm/glm.hpp>

class PureColorMaterial : public Material
{
public:
    glm::vec3 color;

    PureColorMaterial(const glm::vec3 &color);

    void applyMaterial(Shader &shader) const override;
};