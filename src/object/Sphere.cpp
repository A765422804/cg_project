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
    vertices.clear();
    indices.clear();

    // 球面坐标系公式：
    // x = radius * sin(theta) * cos(phi)
    // y = radius * cos(theta)
    // z = radius * sin(theta) * sin(phi)
    // 其中：theta 是极角（0~π），phi 是方位角（0~2π）

    // 遍历所有堆叠（stacks，极角方向）
    for (unsigned int i = 0; i <= stacks; ++i)
    {
        float theta = glm::pi<float>() * (float)i / (float)stacks; // 极角（0 到 π）
        float sinTheta = sin(theta);
        float cosTheta = cos(theta);

        // 遍历所有切片（slices，方位角方向）
        for (unsigned int j = 0; j <= slices; ++j)
        {
            float phi = 2.0f * glm::pi<float>() * (float)j / (float)slices; // 方位角（0 到 2π）
            float sinPhi = sin(phi);
            float cosPhi = cos(phi);

            // 1. 计算顶点位置（世界空间）
            float x = radius * sinTheta * cosPhi;
            float y = radius * cosTheta;
            float z = radius * sinTheta * sinPhi;

            // 2. 计算法线（球体的法线就是顶点指向球心的反方向，即顶点坐标归一化）
            float nx = x / radius; // 归一化（除以半径得到单位向量）
            float ny = y / radius;
            float nz = z / radius;

            // 3. 计算纹理坐标（范围 [0,1]）
            float u = (float)j / (float)slices; // u 坐标（左右方向）
            float v = (float)i / (float)stacks; // v 坐标（上下方向）

            // 将位置、法线、纹理坐标加入顶点数组
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
            vertices.push_back(nx);
            vertices.push_back(ny);
            vertices.push_back(nz);
            vertices.push_back(u);
            vertices.push_back(v);
        }
    }

    // 生成索引（每个面由两个三角形组成）
    // 每个堆叠和切片形成一个四边形，拆分为两个三角形
    for (unsigned int i = 0; i < stacks; ++i)
    {
        for (unsigned int j = 0; j < slices; ++j)
        {
            // 当前四边形的四个顶点索引
            unsigned int topLeft = i * (slices + 1) + j;
            unsigned int topRight = topLeft + 1;
            unsigned int bottomLeft = (i + 1) * (slices + 1) + j;
            unsigned int bottomRight = bottomLeft + 1;

            // 第一个三角形：上左 -> 上右 -> 下右
            indices.push_back(topLeft);
            indices.push_back(topRight);
            indices.push_back(bottomRight);

            // 第二个三角形：上左 -> 下右 -> 下左
            indices.push_back(topLeft);
            indices.push_back(bottomRight);
            indices.push_back(bottomLeft);
        }
    }
}

void Sphere::render(glm::mat4 uModel, glm::mat4 uView, glm::mat4 uProjection)
{
    Object::render(uModel, uView, uProjection);

    renderVertex();
}

Sphere::~Sphere()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    delete shader;
}

void Sphere::renderVertex()
{
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0); // 使用索引绘制球体
    glBindVertexArray(0);
}