#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../Shader.h"
#include "../material/Material.h"

class Object
{
public:
    virtual void render(glm::mat4 model, glm::mat4 view, glm::mat4 projection)
    {
        shader->use();

        // 应用材质
        material->applyMaterial(*shader);

        // 传递模型、视图和投影矩阵到着色器
        shader->setMat4("uModel", model);
        shader->setMat4("uView", view);
        shader->setMat4("uProjection", projection);
    }
    virtual void renderVertex() = 0;

    Shader *shader;

    glm::mat4 model;

protected:
    unsigned int VAO, VBO, EBO;
    Material *material;
};