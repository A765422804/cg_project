#include "PhoneMaterial.h"

PhongMaterial::PhongMaterial(const glm::vec3 &diffuse,
                             const glm::vec3 &specular,
                             const glm::vec3 &ambient,
                             float shininess)
    : diffuse(diffuse), specular(specular), ambient(ambient), shininess(shininess)
{
}

void PhongMaterial::applyMaterial(Shader &shader) const
{
    shader.setVec3("uMaterialDiffuse", diffuse);
    shader.setVec3("uMaterialSpecular", specular);
    shader.setVec3("uMaterialAmbient", ambient);
    shader.setFloat("uMaterialShininess", shininess);
    shader.setInt("uUseDiffuseMap", 0);
}