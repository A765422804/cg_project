#include "Cone.h"

Cone::Cone(const char *vertexPath, const char *fragmentPath, Material *material,
           float radius, float height, unsigned int segments)
    : radius(radius), height(height), segments(segments)
{
    shader = new Shader(vertexPath, fragmentPath);
    this->material = material;
    generateConeData();

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // 位置属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    // 法线属性
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // 纹理坐标
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Cone::generateConeData()
{
    vertices.clear();
    indices.clear();

    // 顶点：顶部点
    vertices.insert(vertices.end(), {0.0f, height, 0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 0.5f});

    // 底部中心点
    vertices.insert(vertices.end(), {0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.5f, 0.5f});

    // 底部边缘顶点
    for (unsigned int i = 0; i <= segments; ++i)
    {
        float angle = 2.0f * glm::pi<float>() * i / segments;
        float x = radius * cos(angle);
        float z = radius * sin(angle);

        // 侧表面顶点（位置、法线、纹理）
        vertices.insert(vertices.end(), {x, 0.0f, z,
                                         x / radius, 0.0f, z / radius, // 法线
                                         (float)i / segments, 0.0f});

        // 底表面顶点（位置、法线、纹理）
        vertices.insert(vertices.end(), {x, 0.0f, z,
                                         0.0f, -1.0f, 0.0f, // 法线
                                         (x + radius) / (2 * radius), (z + radius) / (2 * radius)});
    }

    // 生成侧面索引
    for (unsigned int i = 0; i < segments; ++i)
    {
        unsigned int top = 0;
        unsigned int edge = 2 + 2 * i;
        unsigned int nextEdge = 2 + 2 * (i + 1);

        indices.insert(indices.end(), {top, edge, nextEdge});
    }

    // 生成底面索引
    unsigned int bottomCenter = 1;
    for (unsigned int i = 0; i < segments; ++i)
    {
        unsigned int bottomEdge = 3 + 2 * i;
        unsigned int nextBottomEdge = 3 + 2 * (i + 1);
        indices.insert(indices.end(), {bottomCenter, nextBottomEdge, bottomEdge});
    }
}

void Cone::render(glm::mat4 model, glm::mat4 view, glm::mat4 projection)
{
    Object::render(model, view, projection);
    renderVertex();
}

Cone::~Cone()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    delete shader;
}

void Cone::renderVertex()
{
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}