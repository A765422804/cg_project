#include "PureColorMaterial.h"

PureColorMaterial::PureColorMaterial(const glm::vec3& color) : Material(), color(color) 
{}

void PureColorMaterial::applyMaterial(Shader& shader) const
{
    shader.setVec3("uColor", color);
}