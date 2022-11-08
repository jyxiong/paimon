#include "Renderer.h"

#include "glm/gtx/transform.hpp"
#include "rttr/registration"

#include "component/Scene.h"
#include "component/Transform.h"
#include "control/Input.h"
#include "renderer/Camera.h"
#include "renderer/Material.h"
#include "renderer/MeshFilter.h"
#include "renderer/MeshRenderer.h"
#include "utils/Screen.h"

using namespace Paimon;

RTTR_REGISTRATION
{
    rttr::registration::class_<Renderer>("Renderer")
        .constructor<Entity>()(rttr::policy::ctor::as_raw_ptr);
}

Renderer::Renderer(Entity &entity)
    : Component(entity)
{

}

void Renderer::Awake()
{
    //
    m_fishSoup = EntityManager::CreateEntity("fishsoup_pot");

    auto transform = m_fishSoup->AddComponent<Transform>();

    auto meshFilter = m_fishSoup->AddComponent<MeshFilter>();
    meshFilter->LoadMesh("model/fishsoup_pot.mesh");

    auto meshRenderer = m_fishSoup->AddComponent<MeshRenderer>();
    auto material = std::make_shared<Material>();
    material->Parse("material/fishsoup_pot.mat");
    meshRenderer->SetMaterial(material);

    // camera
    m_sceneCamera = EntityManager::CreateEntity("Scene Camera");

    auto cameraTransformComponent = m_sceneCamera->AddComponent<Transform>();
    cameraTransformComponent->SetPosition(glm::vec3(0, 0, 10));

    auto cameraComponent = m_sceneCamera->AddComponent<Camera>();
    cameraComponent->SetDepth(0);
    cameraComponent->SetCullingMask(0x02);

    // camera2
    m_editorCamera = std::make_shared<Entity>("camera2");

    auto cameraTransformComponent2 = m_editorCamera->AddComponent<Transform>();
    cameraTransformComponent2->SetPosition(glm::vec3(1, 0, 10));

    auto cameraComponent2 = m_editorCamera->AddComponent<Camera>();
    cameraComponent2->SetDepth(1);
    cameraComponent2->SetCullingMask(0x01);
    cameraComponent2->SetClearFlag(GL_DEPTH_BUFFER_BIT);

    CameraManager::SetCamera(cameraComponent);
    CameraManager::SetCamera(cameraComponent2);

    //上一帧的鼠标位置
    m_mousePosition = Input::GetMousePosition();
}

void Renderer::Update()
{
    auto cameraComponent = m_sceneCamera->GetComponent<Camera>();
    cameraComponent->SetView(glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    cameraComponent->SetProjection(glm::radians(60.f), Screen::GetAspect(), 1.f, 1000.f);

    auto cameraComponent2 = m_editorCamera->GetComponent<Camera>();
    cameraComponent2->SetView(glm::vec3(m_editorCamera->GetComponent<Transform>()->GetPosition().x, 0, 0),
                              glm::vec3(0, 1, 0));
    cameraComponent2->SetProjection(glm::radians(60.f), Screen::GetAspect(), 1.f, 1000.f);

    // transform model
    if (Input::IsKeyDown(KeyCode::R))
    {
        static float rotateEulerAngle = 0.f;
        rotateEulerAngle += 0.1f;
        auto rotation = m_fishSoup->GetComponent<Transform>()->GetRotation();
        rotation.y = rotateEulerAngle;
        m_fishSoup->GetComponent<Transform>()->SetRotation(rotation);
    }

    // transform camera
    if (Input::IsKeyDown(KeyCode::LeftAlt) && Input::IsMouseButtonDown(MouseButton::MouseButtonLeft))
    {
        auto degree = Input::GetMousePosition().x - m_mousePosition.x;

        auto rotateMat = glm::rotate(glm::mat4(1.0), glm::radians(degree), glm::vec3(0, 1, 0));
        auto position = rotateMat * glm::vec4(m_editorCamera->GetComponent<Transform>()->GetPosition(), 1);
        m_editorCamera->GetComponent<Transform>()->SetPosition(position);
    }
    m_mousePosition = Input::GetMousePosition();

    // camera
    m_editorCamera->GetComponent<Transform>()->SetPosition(
        m_editorCamera->GetComponent<Transform>()->GetPosition() * (10.f - Input::GetMouseScroll()) / 10.f);
}


