#version 410 core
out vec4 FragColor;

in vec3 FragPos;  // 片段位置
in vec3 Normal;   // 片段法线
in vec2 TexCoord; // 纹理坐标

// 公共参数：视点位置
uniform vec3 uViewPos;

// -------------------------- 平行光结构体 --------------------------
struct DirectionalLight {
    vec3 direction;  // 光线方向（例如：vec3(-0.2, -1.0, -0.3)）
    vec3 color;      // 光线颜色（例如：vec3(1.0, 1.0, 1.0)）
};
uniform DirectionalLight uDirLight;  // 平行光

// -------------------------- 点光源结构体 --------------------------
struct PointLight {
    vec3 position;   // 光源位置（对应你的 glm::vec3 position）
    vec3 color;      // 光线颜色（例如：vec3(1.0, 1.0, 0.8) 暖光）
    
    // 衰减参数（完全匹配你的参数）
    float constant;  // 衰减常数（例如：1.0）
    float linear;    // 线性衰减（例如：0.09）
    float quadratic; // 二次衰减（例如：0.032）
};
uniform PointLight uPointLight;  // 点光源

// -------------------------- 材质参数 --------------------------
uniform vec3 uMaterialAmbient;   // 环境光反射率（例如：vec3(0.1)）
uniform vec3 uMaterialSpecular;  // 镜面反射率（例如：vec3(0.5)）
float uMaterialShininess;        // 高光锐度（例如：32.0）

// 纹理控制
uniform bool uUseDiffuseMap;
uniform sampler2D uDiffuseMap;
uniform vec3 uMaterialDiffuse;   // 漫反射颜色（纹理关闭时使用）

// -------------------------- 光照计算函数 --------------------------
// 计算平行光贡献
vec3 calcDirLight(DirectionalLight light, vec3 normal, vec3 viewDir, vec3 baseColor) 
{
    vec3 lightDir = normalize(-light.direction);  // 平行光方向取反（指向光源）
    
    // 漫反射
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * baseColor * light.color;
    
    // 镜面反射
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = spec * uMaterialSpecular * light.color;
    
    // 环境光
    vec3 ambient = uMaterialAmbient * light.color;
    
    return ambient + diffuse + specular;
}

// 计算点光源贡献
vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 baseColor) 
{
    vec3 lightDir = normalize(light.position - fragPos);  // 从片段指向点光源
    
    // 漫反射
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * baseColor * light.color;
    
    // 镜面反射
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), uMaterialShininess);
    vec3 specular = spec * uMaterialSpecular * light.color;
    
    // 环境光
    vec3 ambient = uMaterialAmbient * light.color;
    
    // 衰减计算
    float distance = length(light.position - fragPos);  // 片段到点光源的距离
    float attenuation = 1.0 / (
        light.constant + 
        light.linear * distance + 
        light.quadratic * (distance * distance)
    );
    
    // 应用衰减
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    
    return ambient + diffuse + specular;
}

void main()
{
    // 基础颜色（纹理或材质漫反射色）
    vec3 baseColor = uMaterialDiffuse;
    if (uUseDiffuseMap) 
    {
        baseColor = texture(uDiffuseMap, TexCoord).rgb;
    }
    
    // 公共计算：法线和视线方向
    vec3 normal = normalize(Normal);
    vec3 viewDir = normalize(uViewPos - FragPos);  // 从片段到视点
    
    // 累加两个光源的贡献
    vec3 result = vec3(0.0);
    result += calcDirLight(uDirLight, normal, viewDir, baseColor);  // 平行光
    result += calcPointLight(uPointLight, normal, FragPos, viewDir, baseColor);  // 点光源

    FragColor = vec4(result, 1.0);
}
