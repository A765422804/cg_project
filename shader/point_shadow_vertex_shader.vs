#version 410 core
layout (location = 0) in vec3 aPos;

uniform mat4 uModel;        // 模型矩阵
uniform mat4 uPointVPMatrix; // 点光源的投影+视图矩阵
uniform vec3 uLightPos;     // 点光源位置

out vec3 FragPos; // 顶点到点光源的相对位置（传递给片段着色器）

void main()
{
    vec3 worldPos = (uModel * vec4(aPos, 1.0)).xyz; // 世界空间位置
    FragPos = worldPos - uLightPos;                 // 相对位置
    gl_Position = uPointVPMatrix * vec4(worldPos, 1.0); // 裁剪空间位置（依赖VP矩阵）
}