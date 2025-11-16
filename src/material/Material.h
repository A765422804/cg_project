#pragma once

#include <glm/glm.hpp>
#include "../Shader.h"

class Material
{
public:
    glm::vec3 diffuse;  // 漫反射颜色
    glm::vec3 specular; // 镜面反射颜色
    glm::vec3 ambient;  // 环境光颜色
    float shininess;    // 反射率

    Material(glm::vec3 _diffuse = glm::vec3(1.0f), glm::vec3 _specular = glm::vec3(1.0f),
             glm::vec3 _ambient = glm::vec3(0.1f), float _shininess = 32.0f)
        : diffuse(_diffuse), specular(_specular), ambient(_ambient), shininess(_shininess) {}

    virtual void applyMaterial(Shader &shader) const
    {
        shader.setVec3("uMaterialDiffuse", diffuse);
        shader.setVec3("uMaterialSpecular", specular);
        shader.setVec3("uMaterialAmbient", ambient);
        shader.setFloat("uMaterialShininess", shininess);
    }

    virtual ~Material() {}
};
