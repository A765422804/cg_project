#pragma once

#include "Object.h"

class Plane : public Object
{
public:
    // 构造函数，传入着色器文件路径和材质对象
    Plane(const char *vertexPath, const char *fragmentPath, Material *material);

    // 渲染平面
    void render(glm::mat4 model, glm::mat4 view, glm::mat4 projection);

    // 析构函数
    ~Plane();

    void renderVertex();

private:
    // 平面四个顶点的数据（位置、法线和纹理坐标）
    float vertices[32] = {
        // 位置              // 法线             // 纹理坐标
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // 左下角
        0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,  // 右下角
        0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,   // 右上角
        -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f   // 左上角
    };

    unsigned int indices[6] = {
        0, 1, 2, // 第一个三角形
        0, 2, 3  // 第二个三角形
    };
};
