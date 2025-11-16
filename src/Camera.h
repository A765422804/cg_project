#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>

class Camera
{
public:
    // 相机的基本属性
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;

    // 相机的参数
    float yaw;
    float pitch;
    float movementSpeed;
    float mouseSensitivity;
    float zoom;

    // 投影参数
    float nearPlane;
    float farPlane;

    // 构造函数
    Camera(glm::vec3 startPosition, glm::vec3 startUp, float startYaw, float startPitch,
           float startZoom = 45.0f, float near = 0.1f, float far = 100.0f);

    // 获取相机的视图矩阵
    glm::mat4 GetViewMatrix();

    // 获取相机的投影矩阵
    glm::mat4 GetProjectionMatrix(float aspectRatio);

    // 处理键盘输入
    void ProcessKeyboard(float deltaTime, bool *keys);

    // 处理鼠标输入
    void ProcessMouseMovement(float xOffset, float yOffset, bool constrainPitch = true);

    // 处理滚轮缩放输入
    void ProcessMouseScroll(float yOffset);

private:
    // 更新相机的方向
    void updateCameraVectors();
};
