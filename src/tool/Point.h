#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "../Shader.h"
#include "../material/Material.h"

class Point
{
public:
    Point(const char *vertexPath, const char *fragmentPath, Material *material);

    void render(glm::mat4 uModel, glm::mat4 uView, glm::mat4 uProjection);

    void setPosition(glm::vec3 position) { this->position = position; }

    ~Point();

private:
    unsigned int VBO, VAO;
    Shader *shader;
    Material *material; // 材质对象，用于渲染时应用

    glm::vec3 position;
    float vertices[3];      // 单个点的位置数据
    float pointSize = 20.0f; // 点的大小
};
