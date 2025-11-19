#pragma once

#include "Object.h"

class Sphere : public Object
{
public:
    // 构造函数，传入着色器文件路径和材质对象
    Sphere(const char *vertexPath, const char *fragmentPath, Material *material, float radius = 1.0f, unsigned int stacks = 180, unsigned int slices = 360);

    // 渲染球体
    void render(glm::mat4 uModel, glm::mat4 uView, glm::mat4 uProjection);

    // 析构函数
    ~Sphere();

    void renderVertex();

private:
    float radius;
    unsigned int stacks; // 纬度（经度层次）
    unsigned int slices; // 经度（纬度层次）

    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    void generateSphereData();
};