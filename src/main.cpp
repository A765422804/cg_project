#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <memory>

#include "Camera.h"

#include "light/DirectionalLight.h"
#include "light/PointLight.h"

#include "object/Object.h"
#include "object/Cube.h"
#include "object/Plane.h"
#include "object/Sphere.h"
#include "object/Cone.h"
#include "object/Cylinder.h"

#include "material/PhoneMaterial.h"
#include "material/TexturedPhoneMaterial.h"
#include "material/PureColorMaterial.h"

#include "tool/Line.h"
#include "tool/Point.h"

#include "texture/Texture.h"

// 回调函数
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window, float deltaTime);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

// create scene objects
void createSceneObjects(std::vector<std::shared_ptr<Object>> &objects);

// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 1280;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 15.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);
bool keys[1024];

// frame time
float lastFrame = 0.0f;

// directional light 阴影映射参数
const unsigned int SHADOW_WIDTH = 1024; // 阴影图分辨率（越大越清晰，性能消耗越高）
const unsigned int SHADOW_HEIGHT = 1024;
unsigned int depthMapFBO; // 帧缓冲：用于渲染阴影图
unsigned int depthMap;    // 深度纹理：存储阴影图

// point light 阴影参数
const unsigned int POINT_SHADOW_WIDTH = 2048;
const unsigned int POINT_SHADOW_HEIGHT = 2048;
unsigned int pointDepthMap;    // 立方体贴图深度纹理
unsigned int pointDepthMapFBO; // 帧缓冲（用于渲染6个方向）
float pointLightFar = 15.0f;   // 点光源视锥体范围（需覆盖场景）

// default model
glm::mat4 model = glm::mat4(1.0f);

// debug mode
bool debugMode = false;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "CG-Project", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // 锁定鼠标到窗口中心
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);   // 锁定鼠标
    glfwSetCursorPos(window, SCR_WIDTH / 2.0f, SCR_HEIGHT / 2.0f); // 将光标移动到窗口中心

    // 设置鼠标输入回调
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    if (debugMode)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // GL_LINE for wireframe mode

    // 对象初始化
    // -----------

    // 创建 DirectionalLight（平行光）
    DirectionalLight dirLight(glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(0.7f, 0.7f, 0.7f));

    // 创建 PointLight（点光源）
    PointLight pointLight(glm::vec3(2.0f, 0.0f, 2.0f), glm::vec3(0.9));
    // 点光源旋转参数
    float rotationSpeed = 60.0f;                  // 旋转速度（度/秒）
    float rotationRadius = pointLight.position.z; // 旋转半径（离Y轴的距离）
    float yHeight = pointLight.position.y;        // 点光源的Y轴高度（旋转时保持不变）
    // 创建 point light 实体
    PureColorMaterial pointLightMaterial(glm::vec3(1.0f, 1.0f, 1.0f));
    Cube pointLightCube("../shader/default_vertex_shader.vs", "../shader/default_fragment_shader.fs", &pointLightMaterial);

    // 创建场景物体
    std::vector<std::shared_ptr<Object>> objects;
    createSceneObjects(objects);

    // 创建point和line对象
    PureColorMaterial pointMaterial(glm::vec3(1.0f, 0.0f, 0.0f));
    Point point("../shader/default_vertex_shader.vs", "../shader/default_fragment_shader.fs", &pointMaterial);
    point.setPosition(glm::vec3(0.0f, 0.0f, 0.0f));

    PureColorMaterial lineMaterial(glm::vec3(1.0f, 0.0f, 0.0f));
    Line line_x("../shader/default_vertex_shader.vs", "../shader/default_fragment_shader.fs", glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(10.0f, 0.0f, 0.0f), &lineMaterial);
    Line line_y("../shader/default_vertex_shader.vs", "../shader/default_fragment_shader.fs", glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 10.0f, 0.0f), &lineMaterial);
    Line line_z("../shader/default_vertex_shader.vs", "../shader/default_fragment_shader.fs", glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 10.0f), &lineMaterial);

    // ----- shadow -----
    // directional light 阴影渲染设置
    Shader shadowShader("../shader/shadow_vertex_shader.vs", "../shader/shadow_fragment_shader.fs"); // 阴影渲染专用着色器
    // 1. 创建深度纹理（阴影图）
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    // 阴影图只需要深度信息，不需要颜色
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    // 设置纹理参数：使用 nearest 过滤避免阴影模糊（后期可优化为 PCF 软阴影）
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // 超出阴影图范围的区域视为无阴影（边框颜色设为白色，对应深度1.0）
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    // 2. 创建帧缓冲（仅附加深度纹理，不需要颜色缓冲）
    glGenFramebuffers(1, &depthMapFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    // 将深度纹理附加到帧缓冲的深度附着点
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    // 关闭颜色缓冲（阴影图只需要深度）
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    // 检查帧缓冲是否完整
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Shadow framebuffer is incomplete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // 解绑帧缓冲

    // point light 阴影渲染设置
    Shader pointShadowShader("../shader/point_shadow_vertex_shader.vs", "../shader/point_shadow_fragment_shader.fs"); // 点光源阴影着色器
    // 1. 初始化点光源阴影的立方体贴图
    glGenTextures(1, &pointDepthMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, pointDepthMap);
    // 为6个方向（+X,-X,+Y,-Y,+Z,-Z）各创建一个深度纹理
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, // 6个方向的纹理目标
            0,
            GL_DEPTH_COMPONENT,
            POINT_SHADOW_WIDTH,
            POINT_SHADOW_HEIGHT,
            0,
            GL_DEPTH_COMPONENT,
            GL_FLOAT,
            NULL);
    }
    // 2. 设置立方体贴图参数
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    // 3. 初始化点光源阴影的帧缓冲
    glGenFramebuffers(1, &pointDepthMapFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, pointDepthMapFBO);
    // 将立方体贴图附加到帧缓冲的深度附着点
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, pointDepthMap, 0);
    // 关闭颜色缓冲（只需要深度）
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Point shadow framebuffer incomplete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {

        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // 输入
        processInput(window, deltaTime);

        // 更新点光源位置（绕Y轴旋转）
        // 计算旋转角度（随时间增加，单位：弧度）
        float angle = glm::radians(rotationSpeed * currentFrame);
        // 绕Y轴旋转的坐标公式：x = r*cos(angle), z = r*sin(angle), y保持不变
        pointLight.position.x = rotationRadius * cos(angle);
        pointLight.position.z = rotationRadius * sin(angle);
        pointLight.position.y = yHeight; // 固定Y轴高度
        glm::mat4 modelPointLight = glm::mat4(1.0f);
        modelPointLight = glm::translate(modelPointLight, pointLight.position);
        modelPointLight = glm::scale(modelPointLight, glm::vec3(0.2f, 0.2f, 0.2f));

        // -------------------------- 第一步：渲染阴影图 --------------------------
        // -------------------------- 渲染平行光阴影图 --------------------------
        // 1. 计算光源空间矩阵（平行光用正交投影）
        float near_plane = 1.0f, far_plane = 30.0f; // 光源视锥体范围（覆盖场景）
        glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
        // 光源位置：沿平行光反方向移动一段距离（确保能看到整个场景）
        glm::mat4 lightView = glm::lookAt(
            -dirLight.direction * 10.0f, // 光源位置
            glm::vec3(0.0f, 0.0f, 0.0f), // 看向场景中心
            glm::vec3(0.0f, 1.0f, 0.0f)  // 上方向
        );
        glm::mat4 lightSpaceMatrix = lightProjection * lightView; // 光源空间矩阵

        // 2. 绑定阴影帧缓冲，渲染所有物体到阴影图
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT); // 设置视口为阴影图大小
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT); // 只清除深度缓冲

        // 使用阴影着色器渲染场景中的物体（需要产生阴影的物体）
        shadowShader.use();
        shadowShader.setMat4("uLightSpaceMatrix", lightSpaceMatrix);

        // 渲染平面
        for (auto &obj : objects)
        {
            shadowShader.setMat4("uModel", obj->model);
            obj->renderVertex();
        }

        // 解绑阴影帧缓冲，恢复视口为窗口大小
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

        // -------------------------- 渲染点光源阴影图（6个方向） --------------------------
        // 点光源透视投影矩阵（90度FOV，覆盖6个方向）
        glm::mat4 pointProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, pointLightFar);

        // 6个方向的视图矩阵（从点光源位置看向6个轴方向）
        std::vector<glm::mat4> pointViews = {
            glm::lookAt(pointLight.position, pointLight.position + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),  // +X
            glm::lookAt(pointLight.position, pointLight.position + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)), // -X
            glm::lookAt(pointLight.position, pointLight.position + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),   // +Y
            glm::lookAt(pointLight.position, pointLight.position + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)), // -Y
            glm::lookAt(pointLight.position, pointLight.position + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),  // +Z
            glm::lookAt(pointLight.position, pointLight.position + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))  // -Z
        };

        // 绑定点光源阴影帧缓冲，渲染6个方向
        glViewport(0, 0, POINT_SHADOW_WIDTH, POINT_SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, pointDepthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);

        // 在逐个方向渲染时，添加VP矩阵传递
        pointShadowShader.use();
        pointShadowShader.setVec3("uLightPos", pointLight.position);
        pointShadowShader.setFloat("uPointLightFar", pointLightFar); // 传递远平面

        for (unsigned int i = 0; i < 6; ++i)
        {
            glFramebufferTexture2D(
                GL_FRAMEBUFFER,
                GL_DEPTH_ATTACHMENT,
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                pointDepthMap,
                0);
            glClear(GL_DEPTH_BUFFER_BIT);

            // 计算当前方向的VP矩阵（投影+视图）
            glm::mat4 pointVPMatrix = pointProjection * pointViews[i];
            // 传递VP矩阵给着色器（关键！否则裁剪错误）
            pointShadowShader.setMat4("uPointVPMatrix", pointVPMatrix);

            // 渲染物体
            for (auto &obj : objects)
            {
                pointShadowShader.setMat4("uModel", obj->model);
                obj->renderVertex();
            }
        }

        // 恢复默认帧缓冲和视口
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

        // -------------------------- 第二步：正常渲染场景（带阴影） --------------------------
        // render
        // ------

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 清除颜色和深度缓冲区

        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = camera.GetProjectionMatrix((float)SCR_WIDTH / (float)SCR_HEIGHT);

        // 渲染点光源立方体
        pointLightCube.render(modelPointLight, view, projection);

        for (auto &obj : objects)
        {
            obj->shader->use();
            obj->shader->setMat4("uLightSpaceMatrix", lightSpaceMatrix);
            obj->shader->setInt("uShadowMap", 1);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, depthMap);

            obj->shader->setInt("uPointShadowMap", 2);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_CUBE_MAP, pointDepthMap);
            obj->shader->setVec3("uPointLightPos", pointLight.position);
            obj->shader->setFloat("uPointLightFar", pointLightFar);

            dirLight.applyLight(*(obj->shader), "dirLight");
            pointLight.applyLight(*(obj->shader), "pointLight");
            obj->shader->setVec3("uViewPos", camera.position);
            obj->render(obj->model, view, projection);
        }

        // 渲染点和线(debug mode)
        if (debugMode)
        {
            point.render(model, view, projection);
            line_x.render(model, view, projection);
            line_y.render(model, view, projection);
            line_z.render(model, view, projection);
        }

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window, float deltaTime)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // 更新键盘输入
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        keys[GLFW_KEY_W] = true;
    }
    else
        keys[GLFW_KEY_W] = false;

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        keys[GLFW_KEY_S] = true;
    else
        keys[GLFW_KEY_S] = false;

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        keys[GLFW_KEY_A] = true;
    else
        keys[GLFW_KEY_A] = false;

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        keys[GLFW_KEY_D] = true;
    else
        keys[GLFW_KEY_D] = false;

    // 处理相机键盘输入
    camera.ProcessKeyboard(deltaTime, keys);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// 鼠标输入回调：更新相机的yaw和pitch
void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    // 如果是第一次调用，初始化鼠标位置
    static bool firstMouse = true;
    static float lastX = 0;
    static float lastY = 0;
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    // 计算鼠标的偏移量
    float xOffset = xpos - lastX;
    float yOffset = lastY - ypos; // 反向y坐标

    lastX = xpos;
    lastY = ypos;

    // 处理相机的鼠标移动
    camera.ProcessMouseMovement(xOffset, yOffset);
}

// 处理滚轮输入：更新相机的zoom（视场角度）
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

void createSceneObjects(std::vector<std::shared_ptr<Object>> &objects)
{
    // 场景整体尺寸
    const float boxSize = 5.0f;    // X/Z 方向半径
    const float boxHeight = 10.0f; // Y 方向高度
    const float halfHeight = boxHeight / 2.0f;

    // -------------------------- 材质 & 纹理（static 确保整个程序存活） --------------------------
    // 盒子墙体：白/红/绿
    static PhongMaterial whiteMaterial(
        glm::vec3(0.7f, 0.7f, 0.7f),
        glm::vec3(0.7f, 0.7f, 0.7f),
        glm::vec3(0.1f, 0.1f, 0.1f),
        128.0f);

    // 地面使用纹理（地板）
    static Texture floorTexture("../texture/floor.jpeg");
    static TexturedPhongMaterial whiteFloorMaterial(
        glm::vec3(0.7f, 0.7f, 0.7f),
        glm::vec3(0.7f, 0.7f, 0.7f),
        glm::vec3(0.1f, 0.1f, 0.1f),
        128.0f,
        &floorTexture);

    // 背后墙用一张纹理
    static Texture wallTexture("../texture/wall.jpg");
    static TexturedPhongMaterial wallMaterial(
        glm::vec3(0.7f, 0.7f, 0.7f),
        glm::vec3(0.7f, 0.7f, 0.7f),
        glm::vec3(0.1f, 0.1f, 0.1f),
        128.0f,
        &wallTexture);

    // 挂在墙上的画
    static Texture paintingTexture("../texture/painting.jpg");
    static TexturedPhongMaterial paintingMaterial(
        glm::vec3(0.7f, 0.7f, 0.7f),
        glm::vec3(0.7f, 0.7f, 0.7f),
        glm::vec3(0.1f, 0.1f, 0.1f),
        128.0f,
        &paintingTexture);

    static PhongMaterial redMaterial(
        glm::vec3(0.6f, 0.1f, 0.1f),
        glm::vec3(0.6f, 0.1f, 0.1f),
        glm::vec3(0.1f, 0.1f, 0.1f),
        128.0f);

    static PhongMaterial greenMaterial(
        glm::vec3(0.1f, 0.6f, 0.1f),
        glm::vec3(0.1f, 0.6f, 0.1f),
        glm::vec3(0.1f, 0.1f, 0.1f),
        128.0f);

    // 物体材质：多种颜色
    static PhongMaterial blueMaterial(
        glm::vec3(0.1f, 0.1f, 0.8f),
        glm::vec3(0.3f, 0.3f, 1.0f),
        glm::vec3(0.2f, 0.2f, 0.2f),
        64.0f);

    static PhongMaterial yellowMaterial(
        glm::vec3(0.8f, 0.8f, 0.1f),
        glm::vec3(1.0f, 1.0f, 0.3f),
        glm::vec3(0.2f, 0.2f, 0.2f),
        32.0f);

    static PhongMaterial purpleMaterial(
        glm::vec3(0.6f, 0.1f, 0.6f),
        glm::vec3(0.8f, 0.3f, 0.8f),
        glm::vec3(0.3f, 0.3f, 0.3f),
        128.0f);

    static PhongMaterial orangeMaterial(
        glm::vec3(0.8f, 0.4f, 0.1f),
        glm::vec3(1.0f, 0.5f, 0.2f),
        glm::vec3(0.2f, 0.2f, 0.2f),
        64.0f);

    // 地球纹理材质（给一个球用 earth.jpg）
    static Texture earthTex("../texture/earth.jpg");
    static TexturedPhongMaterial earthMaterial(
        glm::vec3(0.2f, 0.2f, 0.2f),
        glm::vec3(1.0f, 1.0f, 1.0f), // 让纹理本身主导漫反射
        glm::vec3(0.3f, 0.3f, 0.3f),
        64.0f,
        &earthTex);

    // 纹理立方体
    static Texture woodTex("../texture/wood.jpg");
    static TexturedPhongMaterial woodMaterial(
        glm::vec3(0.2f, 0.2f, 0.2f),
        glm::vec3(0.8f, 0.8f, 0.8f),
        glm::vec3(0.1f, 0.1f, 0.1f),
        16.0f,
        &woodTex);

    // -------------------------- 康奈尔盒：地面/顶面/墙 --------------------------
    // 1. 地面（Y=-halfHeight，朝上）
    {
        auto floor = std::make_shared<Plane>(
            "../shader/phone_vertex_shader.vs",
            "../shader/phone_fragment_shader.fs",
            &whiteFloorMaterial);
        glm::mat4 m(1.0f);
        m = glm::translate(m, glm::vec3(0.0f, -halfHeight, 0.0f));
        m = glm::rotate(m, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        m = glm::scale(m, glm::vec3(2 * boxSize, 2 * boxSize, 1.0f));
        floor->model = m;
        objects.push_back(floor);
    }

    // 2. 顶面（Y=halfHeight，朝下）
    {
        auto ceiling = std::make_shared<Plane>(
            "../shader/phone_vertex_shader.vs",
            "../shader/phone_fragment_shader.fs",
            &whiteMaterial);
        glm::mat4 m(1.0f);
        m = glm::translate(m, glm::vec3(0.0f, halfHeight, 0.0f));
        m = glm::rotate(m, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        m = glm::scale(m, glm::vec3(2 * boxSize, 2 * boxSize, 1.0f));
        ceiling->model = m;
        objects.push_back(ceiling);
    }

    // 3. 左墙（X=-boxSize，朝右）
    {
        auto leftWall = std::make_shared<Plane>(
            "../shader/phone_vertex_shader.vs",
            "../shader/phone_fragment_shader.fs",
            &redMaterial);
        glm::mat4 m(1.0f);
        m = glm::translate(m, glm::vec3(-boxSize, 0.0f, 0.0f));
        m = glm::rotate(m, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        m = glm::scale(m, glm::vec3(boxHeight, boxHeight, 1.0f));
        leftWall->model = m;
        objects.push_back(leftWall);
    }

    // 4. 右墙（X=boxSize，朝左）
    {
        auto rightWall = std::make_shared<Plane>(
            "../shader/phone_vertex_shader.vs",
            "../shader/phone_fragment_shader.fs",
            &greenMaterial);
        glm::mat4 m(1.0f);
        m = glm::translate(m, glm::vec3(boxSize, 0.0f, 0.0f));
        m = glm::rotate(m, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        m = glm::scale(m, glm::vec3(boxHeight, boxHeight, 1.0f));
        rightWall->model = m;
        objects.push_back(rightWall);
    }

    // 5. 后墙（Z=-boxSize，朝前）
    {
        auto backWall = std::make_shared<Plane>(
            "../shader/phone_vertex_shader.vs",
            "../shader/phone_fragment_shader.fs",
            &wallMaterial);
        glm::mat4 m(1.0f);
        m = glm::translate(m, glm::vec3(0.0f, 0.0f, -boxSize));
        m = glm::scale(m, glm::vec3(2 * boxSize, boxHeight, 1.0f));
        backWall->model = m;
        objects.push_back(backWall);
    }

    // 墙上的画
    {
        auto paintingPlane = std::make_shared<Plane>(
            "../shader/phone_vertex_shader.vs",
            "../shader/phone_fragment_shader.fs",
            &paintingMaterial);
        glm::mat4 m(1.0f);
        m = glm::translate(m, glm::vec3(0.0f, 0.0f, -boxSize + 0.1f));
        m = glm::scale(m, glm::vec3(3.2f, 2.0f, 1.0f));
        paintingPlane->model = m;
        objects.push_back(paintingPlane);
    }

    // -------------------------- 几何体：Cube / Sphere / Cylinder / Cone / Plane --------------------------
    // 中央一根柱（Cylinder）
    {
        auto centerCylinder = std::make_shared<Cylinder>(
            "../shader/phone_vertex_shader.vs",
            "../shader/phone_fragment_shader.fs",
            &yellowMaterial);
        glm::mat4 m(1.0f);
        m = glm::translate(m, glm::vec3(0.0f, -halfHeight + 1.8f, -boxSize * 0.1f));
        m = glm::scale(m, glm::vec3(0.7f, 1.8f, 0.7f));
        centerCylinder->model = m;
        objects.push_back(centerCylinder);
    }

    // 左前方倾斜圆锥（Cone）
    {
        auto frontLeftCone = std::make_shared<Cone>(
            "../shader/phone_vertex_shader.vs",
            "../shader/phone_fragment_shader.fs",
            &purpleMaterial);
        glm::mat4 m(1.0f);
        m = glm::translate(m, glm::vec3(-boxSize * 0.8f, -halfHeight + 0.9f, boxSize * 0.5f));
        m = glm::rotate(m, glm::radians(-10.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        m = glm::rotate(m, glm::radians(8.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        m = glm::scale(m, glm::vec3(0.8f, 1.6f, 0.8f));
        frontLeftCone->model = m;
        objects.push_back(frontLeftCone);
    }

    // 右后角矮胖圆柱（Cylinder）
    {
        auto backRightCylinder = std::make_shared<Cylinder>(
            "../shader/phone_vertex_shader.vs",
            "../shader/phone_fragment_shader.fs",
            &blueMaterial);
        glm::mat4 m(1.0f);
        m = glm::translate(m, glm::vec3(boxSize * 0.6f, -halfHeight + 0.6f, -boxSize * 0.7f));
        m = glm::scale(m, glm::vec3(0.9f, 0.6f, 0.9f));
        backRightCylinder->model = m;
        objects.push_back(backRightCylinder);
    }

    // 右前方高瘦圆锥（Cone）
    {
        auto frontRightCone = std::make_shared<Cone>(
            "../shader/phone_vertex_shader.vs",
            "../shader/phone_fragment_shader.fs",
            &orangeMaterial);
        glm::mat4 m(1.0f);
        m = glm::translate(m, glm::vec3(boxSize * 0.8f, -halfHeight + 1.2f, boxSize * 0.5f));
        m = glm::scale(m, glm::vec3(0.5f, 2.0f, 0.5f));
        frontRightCone->model = m;
        objects.push_back(frontRightCone);
    }

    // 左中位置大立方体（Cube）
    {
        auto bigBlueCube = std::make_shared<Cube>(
            "../shader/phone_vertex_shader.vs",
            "../shader/phone_fragment_shader.fs",
            &blueMaterial);
        glm::mat4 m(1.0f);
        m = glm::translate(m, glm::vec3(-boxSize * 0.4f, -halfHeight + 1.5f, -boxSize * 0.4f));
        m = glm::scale(m, glm::vec3(1.4f, 1.5f, 1.4f));
        bigBlueCube->model = m;
        objects.push_back(bigBlueCube);
    }

    // 中间偏右旋转立方体（Cube）
    {
        auto tiltedCube = std::make_shared<Cube>(
            "../shader/phone_vertex_shader.vs",
            "../shader/phone_fragment_shader.fs",
            &yellowMaterial);
        glm::mat4 m(1.0f);
        m = glm::translate(m, glm::vec3(boxSize * 0.4f, -halfHeight + 1.0f, -boxSize * 0.1f));
        m = glm::rotate(m, glm::radians(25.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        m = glm::scale(m, glm::vec3(1.0f, 1.0f, 1.0f));
        tiltedCube->model = m;
        objects.push_back(tiltedCube);
    }

    // 左中大球（Sphere）
    {
        auto bigPurpleSphere = std::make_shared<Sphere>(
            "../shader/phone_vertex_shader.vs",
            "../shader/phone_fragment_shader.fs",
            &purpleMaterial);
        glm::mat4 m(1.0f);
        m = glm::translate(m, glm::vec3(-boxSize * 0.2f, -halfHeight + 1.0f, 0.0f));
        m = glm::scale(m, glm::vec3(1.0f, 1.0f, 1.0f));
        bigPurpleSphere->model = m;
        objects.push_back(bigPurpleSphere);
    }

    // 右后中球（Sphere）
    {
        auto midOrangeSphere = std::make_shared<Sphere>(
            "../shader/phone_vertex_shader.vs",
            "../shader/phone_fragment_shader.fs",
            &orangeMaterial);
        glm::mat4 m(1.0f);
        m = glm::translate(m, glm::vec3(boxSize * 0.1f, -halfHeight + 0.7f, -boxSize * 0.6f));
        m = glm::scale(m, glm::vec3(0.7f, 0.7f, 0.7f));
        midOrangeSphere->model = m;
        objects.push_back(midOrangeSphere);
    }

    // 左后角迷你球（Sphere）
    {
        auto tinySphere = std::make_shared<Sphere>(
            "../shader/phone_vertex_shader.vs",
            "../shader/phone_fragment_shader.fs",
            &yellowMaterial);
        glm::mat4 m(1.0f);
        m = glm::translate(m, glm::vec3(-boxSize * 0.7f, -halfHeight + 0.25f, -boxSize * 0.7f));
        m = glm::scale(m, glm::vec3(0.25f, 0.25f, 0.25f));
        tinySphere->model = m;
        objects.push_back(tinySphere);
    }

    // 地球球（贴 earth.jpg，略微悬空）
    {
        auto earthSphere = std::make_shared<Sphere>(
            "../shader/phone_vertex_shader.vs",
            "../shader/phone_fragment_shader.fs",
            &earthMaterial);
        glm::mat4 m(1.0f);
        m = glm::translate(m, glm::vec3(boxSize * 0.15f, -halfHeight + 1.2f, boxSize * 0.5f));
        m = glm::scale(m, glm::vec3(0.6f, 0.6f, 0.6f));
        earthSphere->model = m;
        objects.push_back(earthSphere);
    }

    // 小锥体堆叠在中后部
    {

        auto headCone = std::make_shared<Cone>(
            "../shader/phone_vertex_shader.vs",
            "../shader/phone_fragment_shader.fs",
            &purpleMaterial);
        glm::mat4 m2(1.0f);
        m2 = glm::translate(m2, glm::vec3(-boxSize * 0.8f, -halfHeight + 1.3f, -boxSize * 0.5f));
        m2 = glm::scale(m2, glm::vec3(0.5f, 0.9f, 0.5f));
        headCone->model = m2;
        objects.push_back(headCone);
    }

    // 纹理立方体（木箱）
    {
        auto woodCube = std::make_shared<Cube>(
            "../shader/phone_vertex_shader.vs",
            "../shader/phone_fragment_shader.fs",
            &woodMaterial);
        glm::mat4 m(1.0f);
        m = glm::translate(m, glm::vec3(-boxSize * 0.5f, -halfHeight + 0.8f, boxSize * 0.6f));
        m = glm::scale(m, glm::vec3(0.9f, 0.7f, 0.9f));
        woodCube->model = m;
        objects.push_back(woodCube);
    }

    // 前中偏左一个小球
    {
        auto smallBlueSphere = std::make_shared<Sphere>(
            "../shader/phone_vertex_shader.vs",
            "../shader/phone_fragment_shader.fs",
            &blueMaterial);
        glm::mat4 m(1.0f);
        m = glm::translate(m, glm::vec3(-boxSize * 0.2f, -halfHeight + 0.5f, boxSize * 0.9f));
        m = glm::scale(m, glm::vec3(0.4f, 0.4f, 0.4f));
        smallBlueSphere->model = m;
        objects.push_back(smallBlueSphere);
    }

    // 右中偏前一个细长立方体
    {
        auto slimGreenCube = std::make_shared<Cube>(
            "../shader/phone_vertex_shader.vs",
            "../shader/phone_fragment_shader.fs",
            &greenMaterial);
        glm::mat4 m(1.0f);
        m = glm::translate(m, glm::vec3(boxSize * 0.75f, -halfHeight + 1.0f, boxSize * 0.1f));
        m = glm::scale(m, glm::vec3(0.4f, 1.4f, 0.4f));
        slimGreenCube->model = m;
        objects.push_back(slimGreenCube);
    }
}
