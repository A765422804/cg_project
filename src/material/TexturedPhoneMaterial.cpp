#include "TexturedPhoneMaterial.h"

TexturedPhongMaterial::TexturedPhongMaterial(const glm::vec3 &diffuse,
                                             const glm::vec3 &specular,
                                             const glm::vec3 &ambient,
                                             float shininess,
                                             Texture *diffuseMap)
    : PhongMaterial(diffuse, specular, ambient, shininess),
      diffuseMap(diffuseMap)
{
}

void TexturedPhongMaterial::applyMaterial(Shader &shader) const
{
    // 先把 Phong 的基本参数传进去
    PhongMaterial::applyMaterial(shader);

    // 告诉 shader：启用 diffuse 贴图
    shader.setInt("uUseDiffuseMap", 1);

    if (diffuseMap)
    {
        diffuseMap->bind(0);             // 绑定到纹理单元 0
        shader.setInt("uDiffuseMap", 0); // sampler2D 用第 0 个纹理单元
    }
}