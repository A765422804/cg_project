#include "Camera.h"

// 相机默认速度与鼠标灵敏度
const float DEFAULT_SPEED = 3.0f;
const float DEFAULT_SENSITIVITY = 0.1f;
const float DEFAULT_ZOOM = 45.0f;

// 构造函数
Camera::Camera(glm::vec3 startPosition, glm::vec3 startUp, float startYaw, float startPitch,
               float startZoom, float near, float far)
    : front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(DEFAULT_SPEED), mouseSensitivity(DEFAULT_SENSITIVITY), zoom(startZoom),
      nearPlane(near), farPlane(far)
{
    position = startPosition;
    worldUp = startUp;
    yaw = startYaw;
    pitch = startPitch;
    updateCameraVectors();
}

// 获取视图矩阵
glm::mat4 Camera::GetViewMatrix()
{
    return glm::lookAt(position, position + front, up);
}

// 获取投影矩阵
glm::mat4 Camera::GetProjectionMatrix(float aspectRatio)
{
    return glm::perspective(glm::radians(zoom), aspectRatio, nearPlane, farPlane);
}

// 处理键盘输入
void Camera::ProcessKeyboard(float deltaTime, bool *keys)
{
    float velocity = movementSpeed * deltaTime;

    if (keys[GLFW_KEY_W])
        position += front * velocity;
    if (keys[GLFW_KEY_S])
        position -= front * velocity;
    if (keys[GLFW_KEY_A])
        position -= right * velocity;
    if (keys[GLFW_KEY_D])
        position += right * velocity;
}

// 处理鼠标输入
void Camera::ProcessMouseMovement(float xOffset, float yOffset, bool constrainPitch)
{
    xOffset *= mouseSensitivity;
    yOffset *= mouseSensitivity;

    yaw += xOffset;
    pitch += yOffset;

    if (constrainPitch)
    {
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;
    }

    updateCameraVectors();
}

// 处理滚轮输入
void Camera::ProcessMouseScroll(float yOffset)
{
    zoom -= (float)yOffset;
    if (zoom < 1.0f)
        zoom = 1.0f;
    if (zoom > 45.0f)
        zoom = 45.0f;
}

// 更新相机的方向
void Camera::updateCameraVectors()
{
    // 计算新的前向向量
    glm::vec3 newFront;
    newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    newFront.y = sin(glm::radians(pitch));
    newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(newFront);

    // 重新计算右向量
    right = glm::normalize(glm::cross(front, worldUp));

    // 重新计算上向量
    up = glm::normalize(glm::cross(right, front));
}
