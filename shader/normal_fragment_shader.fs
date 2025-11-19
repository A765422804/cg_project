#version 410 core

// 从顶点着色器接收的世界空间法线
in vec3 FragNormal;

// 输出最终颜色
out vec4 FragColor;

void main()
{
    // 1. 法线归一化（确保方向准确）
    vec3 normal = normalize(FragNormal);
    
    // 2. 将法线分量[-1,1]映射到颜色[0,1]（核心可视化逻辑）
    // X轴正方向→红色，负方向→暗红；Y轴正方向→绿色，负方向→暗绿；Z轴正方向→蓝色，负方向→暗蓝
    vec3 normalColor = normal * 0.5 + 0.5;
    
    // 3. 增强对比度（让正负方向差异更明显）
    normalColor = pow(normalColor, vec3(1.2)); // 轻微提亮正方向，压暗负方向
    
    // 4. 输出颜色（不透明度为1.0）
    FragColor = vec4(normalColor, 1.0);
}