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

// 纹理控制
uniform bool uUseDiffuseMap;
uniform sampler2D uDiffuseMap;

void main()
{
    // 归一化法线和光线方向
    vec3 N = normalize(Normal);
    vec3 L = normalize(-uLightDirection); // 平行光方向

    // 1. 基础颜色：来自材质颜色或者纹理
    vec3 baseColor = uMaterialDiffuse;
    if (uUseDiffuseMap)
    {
        baseColor = texture(uDiffuseMap, TexCoord).rgb;
    }

    // 2. 漫反射
    float diff = max(dot(N, L), 0.0);
    vec3 diffuse = diff * baseColor * uLightColor;

    // 3. 镜面反射
    vec3 V = normalize(uViewPos - FragPos);
    vec3 R = reflect(-L, N);
    float spec = pow(max(dot(V, R), 0.0), uMaterialShininess);
    vec3 specular = spec * uMaterialSpecular * uLightColor;

    // 4. 环境光
    vec3 ambient = uMaterialAmbient * uLightColor;

    // 5. 最终结果
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);

    //debug
    // vec3 n = normalize(Normal);
    // vec3 color = n * 0.5 + 0.5;
    // FragColor = vec4(color, 1.0);
}
