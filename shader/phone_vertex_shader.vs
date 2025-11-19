#version 410 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform mat4 uLightSpaceMatrix;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;
out vec4 FragPosLightSpace; 

void main()
{
    // 计算片段的世界坐标和法线
    FragPos = vec3(uModel * vec4(aPos, 1.0));
    Normal = normalize(mat3(transpose(inverse(uModel))) * aNormal);  // 处理法线
    TexCoord = aTexCoord;

    // 计算片段在光源视角中的坐标
    FragPosLightSpace = uLightSpaceMatrix * vec4(FragPos, 1.0);

    // 计算最终位置
    gl_Position = uProjection * uView * uModel * vec4(aPos, 1.0);
}
