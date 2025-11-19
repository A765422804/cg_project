#version 410 core
layout (location = 0) in vec3 aPos;

uniform mat4 uLightSpaceMatrix;  // 光源空间矩阵 = 光源投影矩阵 × 光源视图矩阵
uniform mat4 uModel;             // 模型矩阵

void main()
{
    // 输出顶点在光源空间中的位置（用于深度比较）
    gl_Position = uLightSpaceMatrix * uModel * vec4(aPos, 1.0);
}