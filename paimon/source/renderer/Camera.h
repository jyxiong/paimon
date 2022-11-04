#pragma once

#include <functional>

#include "glm/glm.hpp"

#include "component/Component.h"
#include "component/GameObject.h"

namespace Paimon
{

class Camera : public Component
{
public:
    explicit Camera(GameObject &gameObject);
    ~Camera() override = default;

    void SetView(const glm::vec3 &cameraForward, const glm::vec3 &cameraUp);
    const glm::mat4 &GetView() { return m_view; }

    void SetProjection(float fov, float aspect, float near, float far);
    const glm::mat4 &GetProjection() { return m_projection; }

    void SetClearColor(const glm::vec4 &clearColor) { m_clearColor = clearColor; }

    void SetClearFlag(unsigned int clearFlag) { m_clearFlag = clearFlag; }

    void SetDepth(unsigned char depth) { m_depth = depth; };
    unsigned char GetDepth() const { return m_depth; };

    void SetCullingMask(unsigned char layer) { m_cullingMask = layer; }
    unsigned char GetCullingMask() const { return m_cullingMask; }

    void Clear() const;
private:
    glm::mat4 m_view{};
    glm::mat4 m_projection{};

    glm::vec4 m_clearColor{};
    unsigned int m_clearFlag{};

    unsigned char m_depth{};
    unsigned char m_cullingMask{};

}; // class Camera

class CameraManager
{
public:
    static void SetCamera(const std::shared_ptr<Camera> &camera);
    static void Foreach(const std::function<void()> &func);
    static std::shared_ptr<Camera> GetCurrentCamera() { return m_currentCamera; }
    static void Sort();

private:
    static std::vector<std::shared_ptr<Camera>> m_cameras;
    static std::shared_ptr<Camera> m_currentCamera;
}; // class CameraManager

} // namespace Paimon

