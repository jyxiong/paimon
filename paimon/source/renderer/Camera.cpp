#include "Camera.h"

#include <algorithm>
#include <iostream>

#include "glm/gtc/matrix_transform.hpp"
#include "glad/gl.h"
#include "rttr/registration"

#include "component/Transform.h"

using namespace Paimon;

RTTR_REGISTRATION
{
    rttr::registration::class_<Camera>("Camera")
        .constructor<GameObject>()(rttr::policy::ctor::as_raw_ptr);
}

Camera::Camera(GameObject &gameObject)
    : Component(gameObject), m_cullingMask(0x01)
{

}


void Camera::SetView(const glm::vec3 &cameraForward, const glm::vec3 &cameraUp)
{
    auto transform = std::dynamic_pointer_cast<Transform>(GetGameObject().GetComponent("Transform"));
    m_view = glm::lookAt(transform->GetPosition(), cameraForward, cameraUp);
}

void Camera::SetProjection(float fov, float aspect, float near, float far)
{
    m_projection = glm::perspective(fov, aspect, near, far);
}

void Camera::Clear() const
{
    glClearColor(m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a);
    glClear(static_cast<unsigned int>(m_clearFlag));
}

std::vector<std::shared_ptr<Camera>> CameraManager::m_cameras;
std::shared_ptr<Camera> CameraManager::m_currentCamera;

void CameraManager::SetCamera(const std::shared_ptr<Camera> &camera)
{
    m_cameras.emplace_back(camera);
}

void CameraManager::Foreach(const std::function<void()> &func)
{
    for (auto &camera : m_cameras)
    {
        m_currentCamera = camera;
        m_currentCamera->Clear();
        func();
    }
}

void CameraManager::Sort()
{
    std::sort(m_cameras.begin(),
              m_cameras.end(),
              [](const std::shared_ptr<Camera> &a, const std::shared_ptr<Camera> &b) -> bool {
                  return a->GetDepth() < b->GetDepth();
              });
}
