#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "../Shader.h"
#include "../material/Material.h"

class Line
{
public:
    Line(const char* vertexPath, const char* fragmentPath, glm::vec3 start, glm::vec3 end, Material *material);

    void render(glm::mat4 uModel, glm::mat4 uView, glm::mat4 uProjection);

    ~Line();

private:
    unsigned int VBO, VAO;
    Shader* shader;
    Material *material; // 材质对象，用于渲染时应用

    glm::vec3 start, end;
    float vertices[6];  // 线段两个端点的坐标数据
    float lineWidth = 5.0f; // 线的宽度
};
