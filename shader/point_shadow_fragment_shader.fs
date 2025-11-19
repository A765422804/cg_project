#version 410 core
in vec3 FragPos; // 顶点到点光源的相对位置
uniform float uPointLightFar; // 点光源远平面（=25.0）

void main()
{
    float distance = length(FragPos); // 计算距离
    distance /= uPointLightFar;       // 归一化到[0,1]
    gl_FragDepth = distance;          // 存储深度
}