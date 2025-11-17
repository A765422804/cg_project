#version 410 core

// 输入顶点数据（位置和法线）
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

// 输出到片段着色器的法线
out vec3 FragNormal;

// 模型、视图、投影矩阵
uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

void main()
{
    // 计算顶点世界空间位置
    gl_Position = uProjection * uView * uModel * vec4(aPos, 1.0);
    
    // 计算世界空间法线（使用模型矩阵的逆转置矩阵确保正确变换）
    mat3 normalMatrix = transpose(inverse(mat3(uModel)));
    FragNormal = normalMatrix * aNormal;
}