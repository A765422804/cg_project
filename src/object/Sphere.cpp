#include "Sphere.h"

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
    // 顶点数据的生成：根据经纬度生成球体的顶点
    for (unsigned int i = 0; i <= stacks; ++i)
    {
        float lat = glm::pi<float>() * float(i) / float(stacks); // 纬度
        for (unsigned int j = 0; j <= slices; ++j)
        {
            float lon = 2 * glm::pi<float>() * float(j) / float(slices); // 经度

            // 计算顶点的 x, y, z 坐标
            float x = radius * sin(lat) * cos(lon);
            float y = radius * cos(lat);
            float z = radius * sin(lat) * sin(lon);

            // 纹理坐标（经度和纬度）
            float u = float(j) / float(slices);
            float v = float(i) / float(stacks);

            // 添加顶点到数组（位置 + 法线 + 纹理坐标）
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
            vertices.push_back(x / radius); // 法线（归一化）
            vertices.push_back(y / radius);
            vertices.push_back(z / radius);
            vertices.push_back(u);
            vertices.push_back(v);
        }
    }

    // 索引数据的生成：通过经纬度创建索引，定义两个三角形构成的每个面
    for (unsigned int i = 0; i < stacks; ++i)
    {
        for (unsigned int j = 0; j < slices; ++j)
        {
            unsigned int first = i * (slices + 1) + j;
            unsigned int second = first + slices + 1;

            // 每个面由两个三角形组成
            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(first + 1);

            indices.push_back(second);
            indices.push_back(second + 1);
            indices.push_back(first + 1);
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
