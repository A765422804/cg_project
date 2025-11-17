#pragma once

#include "PhoneMaterial.h"
#include "../texture/Texture.h"

class TexturedPhongMaterial : public PhongMaterial
{
public:
    Texture *diffuseMap; // 不负责 delete，由外面管理生命周期

    TexturedPhongMaterial(const glm::vec3 &diffuse,
                          const glm::vec3 &specular,
                          const glm::vec3 &ambient,
                          float shininess,
                          Texture *diffuseMap);

    void applyMaterial(Shader &shader) const override;
};