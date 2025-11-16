#version 410 core
out vec4 FragColor;

in vec3 FragPos;  // 片段位置
in vec3 Normal;   // 片段法线
in vec2 TexCoord; // 纹理坐标

// 光源的属性
uniform vec3 uLightColor;       // 光源颜色
uniform vec3 uLightDirection;   // 光源方向
uniform vec3 uViewPos;          // 视点位置

// 材质的属性
uniform vec3 uMaterialDiffuse;  // 漫反射颜色
uniform vec3 uMaterialSpecular; // 镜面反射颜色
uniform vec3 uMaterialAmbient;  // 环境光颜色
uniform float uMaterialShininess; // 反射率

void main()
{
    // 1. 漫反射光照计算
    // 计算光源方向并归一化
    vec3 lightDir = normalize(-uLightDirection); // 平行光，所以方向是负的

    // 计算漫反射
    float diff = max(dot(Normal, lightDir), 0.0);  // 漫反射强度

    // 2. 镜面反射光照计算
    // 计算视线方向
    vec3 viewDir = normalize(uViewPos - FragPos);

    // 计算反射方向
    vec3 reflectDir = reflect(-lightDir, Normal);

    // 计算镜面反射
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), uMaterialShininess);

    // 3. 最终颜色计算
    // 环境光影响
    vec3 ambient = 0.1 * uMaterialAmbient; // 环境光影响

    // 漫反射光照
    vec3 diffuse = diff * uMaterialDiffuse * uLightColor; // 漫反射光照

    // 镜面反射光照
    vec3 specular = spec * uMaterialSpecular * uLightColor; // 镜面反射光照

    // 最终光照颜色
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);  // 最终光照颜色
}
