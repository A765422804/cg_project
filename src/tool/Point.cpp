#include "Point.h"

Point::Point(const char *vertexPath, const char *fragmentPath)
{
    shader = new Shader(vertexPath, fragmentPath);

    // 创建 VAO 和 VBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    // 设置顶点数据
    vertices[0] = position.x;
    vertices[1] = position.y;
    vertices[2] = position.z;

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 顶点属性：位置
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Point::render(glm::mat4 uModel, glm::mat4 uView, glm::mat4 uProjection)
{
    shader->use();

    // 传递MVP矩阵到着色器
    shader->setMat4("uModel", uModel);
    shader->setMat4("uView", uView);
    shader->setMat4("uProjection", uProjection);

    glPointSize(pointSize);

    glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS, 0, 1); // 绘制单个点
    glBindVertexArray(0);
}

Point::~Point()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    delete shader;
}
