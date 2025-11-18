#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "Camera.h"

#include "light/DirectionalLight.h"
#include "light/PointLight.h"

#include "object/Cube.h"
#include "object/Plane.h"
#include "object/Sphere.h"

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

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);
bool keys[1024];

// frame time
float lastFrame = 0.0f;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
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

    // 对象初始化
    // -----------

    // 创建 DirectionalLight（平行光）
    DirectionalLight dirLight(glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(.0f, .0f, .0f));

    // 创建 PointLight（点光源）
    PointLight pointLight(glm::vec3(5.0f, 5.0f, 5.0f), glm::vec3(1.0, 1.0, 1.0));

    // 创建 Cube 对象
    PhongMaterial cubeMaterial(glm::vec3(0.8f, 0.1f, 0.1f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.1f, 0.1f, 0.1f), 32.0f);
    Cube cube("../shader/phone_vertex_shader.vs", "../shader/phone_fragment_shader.fs", &cubeMaterial);

    // 创建point实体
    PureColorMaterial pointLightMaterial(glm::vec3(1.0f, 1.0f, 1.0f));
    Cube pointLightCube("../shader/default_vertex_shader.vs", "../shader/default_fragment_shader.fs", &pointLightMaterial);

    // 创建 Plane 对象，传入 Phong 材质
    Texture floorTex("../texture/floor.jpg");
    TexturedPhongMaterial planeMaterial(glm::vec3(0.1f, 0.1f, 0.8f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.1f, 0.1f, 0.1f), 32.0f, &floorTex);
    Plane plane("../shader/phone_vertex_shader.vs", "../shader/phone_fragment_shader.fs", &planeMaterial);

    // 创建 Sphere 对象
    Texture earthTex("../texture/earth.jpg");
    TexturedPhongMaterial sphereMaterial(glm::vec3(0.1f, 0.8f, 0.1f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.1f, 0.1f, 0.1f), 32.0f, &earthTex);
    Sphere sphere("../shader/phone_vertex_shader.vs", "../shader/phone_fragment_shader.fs", &sphereMaterial);

    // 创建point和line对象
    PureColorMaterial pointMaterial(glm::vec3(1.0f, 1.0f, 0.0f));
    Point point("../shader/default_vertex_shader.vs", "../shader/default_fragment_shader.fs", &pointMaterial);
    point.setPosition(glm::vec3(0.0f, 0.0f, 0.0f));

    PureColorMaterial lineMaterial(glm::vec3(1.0f, 1.0f, 0.0f));
    Line line_x("../shader/default_vertex_shader.vs", "../shader/default_fragment_shader.fs", glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(10.0f, 0.0f, 0.0f), &lineMaterial);
    Line line_y("../shader/default_vertex_shader.vs", "../shader/default_fragment_shader.fs", glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 10.0f, 0.0f), &lineMaterial);
    Line line_z("../shader/default_vertex_shader.vs", "../shader/default_fragment_shader.fs", glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 10.0f), &lineMaterial);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {

        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window, deltaTime);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 清除颜色和深度缓冲区

        // 新增：点光源旋转参数
        float rotationSpeed = 30.0f; // 旋转速度（度/秒）
        float rotationRadius = 5.0f; // 旋转半径（离Y轴的距离）
        float yHeight = 5.0f;        // 点光源的Y轴高度（旋转时保持不变）
        // 新增：更新点光源位置（绕Y轴旋转）
        // 计算旋转角度（随时间增加，单位：弧度）
        float angle = glm::radians(rotationSpeed * currentFrame);
        // 绕Y轴旋转的坐标公式：x = r*cos(angle), z = r*sin(angle), y保持不变
        pointLight.position.x = rotationRadius * cos(angle);
        pointLight.position.z = rotationRadius * sin(angle);
        pointLight.position.y = yHeight; // 固定Y轴高度

        // 创建MVP
        glm::mat4 modelPlane = glm::mat4(1.0f);
        modelPlane = glm::translate(modelPlane, glm::vec3(0.0f, -1.0f, 0.0f));
        modelPlane = glm::rotate(modelPlane, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        modelPlane = glm::scale(modelPlane, glm::vec3(10.0f, 10.0f, 1.0f));

        glm::mat4 modelSphere = glm::mat4(1.0f);
        modelSphere = glm::translate(modelSphere, glm::vec3(0.0f, 1.0f, 0.0f));

        glm::mat4 modelCube = glm::mat4(1.0f);
        modelCube = glm::translate(modelCube, glm::vec3(2.0f, 1.0f, 0.0f));

        glm::mat4 modelPointLight = glm::mat4(1.0f);
        modelPointLight = glm::translate(modelPointLight, pointLight.position);
        modelPointLight = glm::scale(modelPointLight, glm::vec3(0.1f, 0.1f, 0.1f));

        glm::mat4 model = glm::mat4(1.0f);

        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = camera.GetProjectionMatrix((float)SCR_WIDTH / (float)SCR_HEIGHT);

        // TODO: sphere应该是有点问题
        // TODO: shadow map

        // 渲染点光源立方体
        pointLightCube.render(modelPointLight, view, projection);

        // 渲染sphere
        dirLight.applyLight(*sphere.shader, "dirLight");
        pointLight.applyLight(*sphere.shader, "pointLight");
        sphere.shader->setVec3("uViewPos", camera.position);
        sphere.render(modelSphere, view, projection);

        // 渲染plane
        dirLight.applyLight(*plane.shader, "dirLight");
        pointLight.applyLight(*plane.shader, "pointLight");
        plane.shader->setVec3("uViewPos", camera.position);
        plane.render(modelPlane, view, projection);

        // 渲染cube
        dirLight.applyLight(*cube.shader, "dirLight");
        pointLight.applyLight(*cube.shader, "pointLight");
        cube.shader->setVec3("uViewPos", camera.position);
        cube.render(modelCube, view, projection);

        // 渲染点和线

        point.render(model, view, projection);
        line_x.render(model, view, projection);
        line_y.render(model, view, projection);
        line_z.render(model, view, projection);

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
        keys[GLFW_KEY_W] = true;
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