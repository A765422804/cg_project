#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "../Shader.h"

class Line
{
public:
    Line(const char* vertexPath, const char* fragmentPath, glm::vec3 start, glm::vec3 end);

    void render(glm::mat4 uModel, glm::mat4 uView, glm::mat4 uProjection);

    ~Line();

private:
    unsigned int VBO, VAO;
    Shader* shader;

    glm::vec3 start, end;
    float vertices[6];  // 线段两个端点的坐标数据
    float lineWidth = 5.0f; // 线的宽度
};
