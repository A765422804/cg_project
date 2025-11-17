#include "Sphere.h"

#include <iostream>

Sphere::Sphere(const char *vertexPath, const char *fragmentPath, Material *material, float radius, unsigned int stacks, unsigned int slices)
    : radius(radius), stacks(stacks), slices(slices)
{
    shader = new Shader(vertexPath, fragmentPath);
    this->material = material;

    // 生成球体顶点和索引数据
    generateSphereData();

    // 创建 VAO、VBO 和 EBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    // 创建 VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    // 创建 EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // 顶点属性：位置
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // 顶点属性：法线
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // 顶点属性：纹理坐标
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Sphere::generateSphereData()
{
    // 顶点数据的生成：根据经纬度生成球体的顶点（无冗余）
    for (unsigned int i = 0; i <= stacks; ++i)
    {
        float lat = glm::pi<float>() * float(i) / float(stacks); // 纬度（0到π）
        for (unsigned int j = 0; j < slices; ++j)                // 注意：j < slices（不含slices，避免与j=0重复）
        {
            float lon = 2 * glm::pi<float>() * float(j) / float(slices); // 经度（0到2π）

            // 计算顶点的 x, y, z 坐标
            float x = radius * sin(lat) * cos(lon);
            float y = radius * cos(lat);
            float z = radius * sin(lat) * sin(lon);

            // 纹理坐标（u范围0~1，v范围0~1，无缝衔接）
            float u = float(j) / float(slices);
            float v = float(i) / float(stacks);

            // 添加顶点数据（位置 + 法线 + 纹理坐标）
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
            vertices.push_back(x / radius); // 法线（单位化向量）
            vertices.push_back(y / radius);
            vertices.push_back(z / radius);
            vertices.push_back(u);
            vertices.push_back(v);
        }
    }

    // 索引数据的生成：处理球面网格连接，最后一列无缝衔接第一列
    for (unsigned int i = 0; i < stacks; ++i)
    {
        for (unsigned int j = 0; j < slices; ++j)
        {
            // 当前行顶点索引
            unsigned int currentRow = i * slices;
            // 下一行顶点索引（行数+1）
            unsigned int nextRow = (i + 1) * slices;
            // 当前列索引
            unsigned int currentCol = j;
            // 下一列索引（处理边界：最后一列衔接第一列）
            unsigned int nextCol = (j + 1) % slices;

            // 第一个三角形：(当前行当前列) -> (下一行当前列) -> (当前行下一列)
            indices.push_back(currentRow + currentCol);
            indices.push_back(nextRow + currentCol);
            indices.push_back(currentRow + nextCol);

            // 第二个三角形：(下一行当前列) -> (下一行下一列) -> (当前行下一列)
            indices.push_back(nextRow + currentCol);
            indices.push_back(nextRow + nextCol);
            indices.push_back(currentRow + nextCol);
        }
    }
}

void Sphere::render(glm::mat4 uModel, glm::mat4 uView, glm::mat4 uProjection)
{
    shader->use();

    // 使用材质
    material->applyMaterial(*shader);

    // 传递模型、视图和投影矩阵到着色器
    shader->setMat4("uModel", uModel);
    shader->setMat4("uView", uView);
    shader->setMat4("uProjection", uProjection);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0); // 使用索引绘制球体
    glBindVertexArray(0);
}

Sphere::~Sphere()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    delete shader;
}
