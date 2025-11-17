#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

#include "../Shader.h"
#include "../material/Material.h"

class Sphere
{
public:
    // 构造函数，传入着色器文件路径和材质对象
    Sphere(const char *vertexPath, const char *fragmentPath, Material *material, float radius = 1.0f, unsigned int stacks = 180, unsigned int slices = 360);

    // 渲染球体
    void render(glm::mat4 uModel, glm::mat4 uView, glm::mat4 uProjection);

    // 析构函数
    ~Sphere();

    Shader *shader;

public:
    unsigned int VBO, VAO, EBO;
    Material *material; // 材质对象，用于渲染时应用

    float radius;
    unsigned int stacks; // 纬度（经度层次）
    unsigned int slices; // 经度（纬度层次）

    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    void generateSphereData();
};
