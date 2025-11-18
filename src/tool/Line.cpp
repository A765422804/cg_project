#include "Line.h"

Line::Line(const char *vertexPath, const char *fragmentPath, glm::vec3 start, glm::vec3 end, Material *material)
{
    shader = new Shader(vertexPath, fragmentPath);
    material = material;

    // 创建 VAO 和 VBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    // 设置线段的两个端点
    vertices[0] = start.x;
    vertices[1] = start.y;
    vertices[2] = start.z;
    vertices[3] = end.x;
    vertices[4] = end.y;
    vertices[5] = end.z;

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 顶点属性：位置
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Line::render(glm::mat4 uModel, glm::mat4 uView, glm::mat4 uProjection)
{
    shader->use();

    // 传递MVP矩阵到着色器
    shader->setMat4("uModel", uModel);
    shader->setMat4("uView", uView);
    shader->setMat4("uProjection", uProjection);

    glLineWidth(lineWidth);

    glBindVertexArray(VAO);
    glDrawArrays(GL_LINES, 0, 2); // 绘制线段
    glBindVertexArray(0);
}

Line::~Line()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    delete shader;
}
