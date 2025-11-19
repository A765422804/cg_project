#version 410 core
out vec4 FragColor;

in vec3 FragPos;  // 片段位置
in vec3 Normal;   // 片段法线
in vec2 TexCoord; // 纹理坐标
in vec4 FragPosLightSpace;  // 顶点在光源空间中的位置

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
uniform float uMaterialShininess;        // 高光锐度（例如：32.0）

// 纹理控制
uniform bool uUseDiffuseMap;
uniform sampler2D uDiffuseMap;
uniform vec3 uMaterialDiffuse;   // 漫反射颜色（纹理关闭时使用）

// 平行光阴影
uniform sampler2D uShadowMap;  // 阴影图纹理

// 点光源阴影
uniform samplerCube uPointShadowMap;  // 点光源阴影立方体贴图
uniform vec3 uPointLightPos;         // 点光源位置
uniform float uPointLightFar;        // 点光源视锥体范围（=25.0）

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
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), uMaterialShininess);
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

// 计算软阴影（PCF滤波）
float calculateShadow()
{
    // 1. 转换光源空间坐标到纹理采样坐标（[0,1]范围）
    vec3 projCoords = FragPosLightSpace.xyz / FragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    // 超出阴影图范围 → 无阴影
    if (projCoords.z > 1.0)
        return 0.0;

    // 2. 计算当前片段在光源视角下的深度（加偏移避免阴影acne）
    float currentDepth = projCoords.z;
    vec3 lightDir = normalize(-uDirLight.direction);
    float bias = max(0.05 * (1.0 - dot(Normal, lightDir)), 0.005);

    // 3. PCF核心：对阴影图周围像素采样（5x5范围，可调整）
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(uShadowMap, 0); // 阴影图的像素大小（1/宽，1/高）

    // 遍历周围5x5个像素（可改为3x3提升性能，或7x7更模糊）
    for(int x = -2; x <= 2; ++x)
    {
        for(int y = -2; y <= 2; ++y)
        {
            // 采样周围像素的深度
            float pcfDepth = texture(uShadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            // 统计在阴影中的像素数（当前深度 > 采样深度 + 偏移 → 算阴影）
            shadow += (currentDepth - bias > pcfDepth) ? 1.0 : 0.0;
        }
    }

    // 4. 取平均：阴影比例 = 阴影像素数 / 总采样数
    shadow /= 25.0; // 5x5=25个采样点（3x3则除以9）

    return shadow;
}

float calculatePointShadow(vec3 fragPos)
{
    vec3 fragToLight = fragPos - uPointLightPos;
    float currentDepth = length(fragToLight);
    if (currentDepth > uPointLightFar)
        return 0.0;

    // 动态偏移（基于表面角度和距离）
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(uPointLightPos - fragPos); // 光源指向片段
    float cosTheta = max(dot(normal, lightDir), 0.001); // 表面与光线的夹角余弦值
    float bias = 0.005 + (1.0 - cosTheta) * 0.1; // 倾斜表面（cosTheta小）增大偏移
    bias *= (currentDepth / uPointLightFar); // 远处适当减小偏移（避免过度悬空）
    bias = min(bias, 0.1); // 限制最大偏移

    // 采样范围随距离衰减（避免远处采样过度）
    float sampleRadius = 0.05 + (currentDepth / uPointLightFar) * 0.1;
    sampleRadius = min(sampleRadius, 0.15);

    float shadow = 0.0;
    int samples = 16; // 减少采样数减轻性能压力，同时减少偏移依赖
    mat3 randomRotation = mat3(
        0.945519,  0.325569, 0.0,
        -0.325569, 0.945519, 0.0,
        0.0,       0.0,      1.0
    );

    vec3 sampleOffsets[16] = vec3[] (
        vec3( 0.5381,  0.1856,  0.4339), vec3(-0.2461,  0.4853,  0.4575),
        vec3( 0.1520, -0.4441,  0.4194), vec3(-0.4888, -0.2367,  0.4357),
        vec3( 0.4439,  0.4765, -0.1331), vec3(-0.1340,  0.4956, -0.3606),
        vec3( 0.0883, -0.4925, -0.3580), vec3(-0.4766, -0.2666, -0.3559),
        vec3( 0.4978,  0.0533,  0.3540), vec3(-0.0794,  0.4971,  0.0762),
        vec3( 0.0355, -0.4992,  0.0599), vec3(-0.4933, -0.0948,  0.0864),
        vec3( 0.4281,  0.2715, -0.4235), vec3(-0.2990,  0.3803, -0.4266),
        vec3( 0.2681, -0.3833, -0.4215), vec3(-0.3797, -0.2865, -0.4262)
    );

    for(int i = 0; i < samples; ++i)
    {
        vec3 offset = randomRotation * sampleOffsets[i] * sampleRadius;
        vec3 sampleDir = fragToLight + offset;
        float closestDepth = texture(uPointShadowMap, sampleDir).r * uPointLightFar;
        
        // 增加深度比较的容差（避免过度敏感）
        shadow += (currentDepth - bias > closestDepth + 0.001) ? 1.0 : 0.0;
    }

    shadow /= float(samples);
    return shadow;
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
    vec3 dirResult =  calcDirLight(uDirLight, normal, viewDir, baseColor);  // 平行光
    vec3 pointResult = calcPointLight(uPointLight, normal, FragPos, viewDir, baseColor);  // 点光源

    // 新增：计算阴影
    float dirShadow = calculateShadow();
    float pointShadow = calculatePointShadow(FragPos); 

    // 合并结果：分别应用阴影
    vec3 totalLight = vec3(0.0);
    totalLight += dirResult * (1.0 - dirShadow); // 应用平行光阴影
    totalLight += pointResult * (1.0 - pointShadow); // 应用点光源阴影

    // 输出最终颜色
    FragColor = vec4(totalLight, 1.0);
}
