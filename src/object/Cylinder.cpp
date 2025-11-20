#include "Cylinder.h"

Cylinder::Cylinder(const char *vertexPath, const char *fragmentPath, Material *material,
                   float radius, float height, unsigned int segments)
    : radius(radius), height(height), segments(segments)
{
    shader = new Shader(vertexPath, fragmentPath);
    this->material = material;
    generateCylinderData();

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // 顶点属性配置（与其他形状一致）
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Cylinder::generateCylinderData()
{
    vertices.clear();
    indices.clear();
    float halfHeight = height / 2.0f;

    // 顶部和底部中心
    vertices.insert(vertices.end(), {
                                        0.0f, halfHeight, 0.0f, // pos
                                        0.0f, 1.0f, 0.0f,       // normal
                                        0.5f, 0.5f              // uv
                                    });                         // index = 0

    vertices.insert(vertices.end(), {
                                        0.0f, -halfHeight, 0.0f, // pos
                                        0.0f, -1.0f, 0.0f,       // normal
                                        0.5f, 0.5f               // uv
                                    });                          // index = 1

    // 侧面 & 顶/底边缘
    for (unsigned int i = 0; i <= segments; ++i)
    {
        float angle = 2.0f * glm::pi<float>() * i / segments;
        float x = radius * cos(angle);
        float z = radius * sin(angle);

        float u = static_cast<float>(i) / segments;

        // 顶面边缘（法线朝上）
        vertices.insert(vertices.end(), {x, halfHeight, z,
                                         0.0f, 1.0f, 0.0f,
                                         u, 1.0f});

        // 底面边缘（法线朝下）
        vertices.insert(vertices.end(), {x, -halfHeight, z,
                                         0.0f, -1.0f, 0.0f,
                                         u, 0.0f});

        // 侧面上顶点（法线沿径向）
        vertices.insert(vertices.end(), {x, halfHeight, z,
                                         x / radius, 0.0f, z / radius,
                                         u, 1.0f});

        // 侧面下顶点
        vertices.insert(vertices.end(), {x, -halfHeight, z,
                                         x / radius, 0.0f, z / radius,
                                         u, 0.0f});
    }

    // 顶部面索引（扇形）
    for (unsigned int i = 0; i < segments; ++i)
    {
        unsigned int center = 0;
        unsigned int currEdge = 2 + 4 * i;
        unsigned int nextEdge = 2 + 4 * (i + 1);
        indices.insert(indices.end(), {center, currEdge, nextEdge});
    }

    // 底部面索引（扇形，注意绕序反过来）
    for (unsigned int i = 0; i < segments; ++i)
    {
        unsigned int center = 1;
        unsigned int currEdge = 3 + 4 * i;
        unsigned int nextEdge = 3 + 4 * (i + 1);
        indices.insert(indices.end(), {center, nextEdge, currEdge});
    }

    // 侧面索引（每段两个三角形）
    for (unsigned int i = 0; i < segments; ++i)
    {
        unsigned int topSide = 4 + 4 * i;              // sideTop(i)
        unsigned int bottomSide = 5 + 4 * i;           // sideBottom(i)
        unsigned int nextTopSide = 4 + 4 * (i + 1);    // sideTop(i+1)
        unsigned int nextBottomSide = 5 + 4 * (i + 1); // sideBottom(i+1)

        indices.insert(indices.end(), {topSide, bottomSide, nextBottomSide});
        indices.insert(indices.end(), {topSide, nextBottomSide, nextTopSide});
    }
}

void Cylinder::render(glm::mat4 model, glm::mat4 view, glm::mat4 projection)
{
    Object::render(model, view, projection);
    renderVertex();
}

Cylinder::~Cylinder()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    delete shader;
}

void Cylinder::renderVertex()
{
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}