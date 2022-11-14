#include "Scene.h"

#include <iostream>

#include <glm/gtx/string_cast.hpp>
#include "rttr/registration"

#include "component/Transform.h"
#include "control/Input.h"
#include "renderer/Camera.h"
#include "renderer/Material.h"
#include "renderer/MeshFilter.h"
#include "renderer/MeshRenderer.h"
#include "renderer/Texture2D.h"
#include "utils/Screen.h"

using namespace Paimon;

RTTR_REGISTRATION
{
    rttr::registration::class_<Scene>("Scene")
        .constructor<Entity>()(rttr::policy::ctor::as_raw_ptr);
}

Scene::Scene(Entity &entity)
    : Component(entity)
{

}

void Scene::Awake()
{
    CreateFishSoupPot();
    CreateQuad();

    // camera
    m_editorCamera = EntityManager::CreateEntity("Scene Camera");

    auto cameraTransformComponent = m_editorCamera->AddComponent<Transform>();
    cameraTransformComponent->SetPosition(glm::vec3(0, 0, 10));

    auto cameraComponent = m_editorCamera->AddComponent<Camera>();
    cameraComponent->SetDepth(0);
    cameraComponent->SetCullingMask(1);

    CameraManager::SetCamera(cameraComponent);

    // 记录最后的鼠标位置
    m_mousePosition = Input::GetMousePosition();
}

void Scene::Update()
{
    auto cameraComponent = m_editorCamera->GetComponent<Camera>();
    cameraComponent->SetView(glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    cameraComponent->SetProjection(glm::radians(60.f), Screen::GetAspect(), 1.f, 1000.f);

    if (Input::IsKeyUp(KeyCode::C))
    {
        auto texture2D = Texture2D::CreateFromTTF("font/hkyuan.ttf", "Hello Paimon!");
        m_fontMaterial->SetTexture("u_diffuse_texture", texture2D);
    }

    // rotate model
    if (Input::IsKeyDown(KeyCode::R))
    {
        static float rotateEulerAngle = 0.f;
        rotateEulerAngle += 0.1f;
        auto rotation = m_fishSoup->GetComponent<Transform>()->GetRotation();
        rotation.y = rotateEulerAngle;
        m_fishSoup->GetComponent<Transform>()->SetRotation(rotation);
    }

    // rotate camera
    if (Input::IsKeyDown(KeyCode::LeftAlt) && Input::IsMouseButtonDown(MouseButton::MouseButtonLeft))
    {
        auto transformComponent = m_editorCamera->GetComponent<Transform>();

        auto degree = Input::GetMousePosition().x - m_mousePosition.x;
        auto rotateMat = glm::rotate(glm::mat4(1.0), glm::radians(degree), glm::vec3(0, 1, 0));
        auto position = rotateMat * glm::vec4(transformComponent->GetPosition(), 1);

        std::cout << glm::to_string(position) << std::endl;

        transformComponent->SetPosition(glm::vec3(position));
    }

    // scale camera
    {
        auto transformComponent = m_editorCamera->GetComponent<Transform>();
        transformComponent->SetPosition(transformComponent->GetPosition() * (10.f - Input::GetMouseScroll()) / 10.f);
    }

    // 更新鼠标位置
    m_mousePosition = Input::GetMousePosition();
}

void Scene::CreateFishSoupPot()
{
    m_fishSoup = EntityManager::CreateEntity("fishsoup_pot");

    auto transform = m_fishSoup->AddComponent<Transform>();

    auto meshFilter = m_fishSoup->AddComponent<MeshFilter>();
    meshFilter->LoadMesh("model/fishsoup_pot.mesh");

    auto meshRenderer = m_fishSoup->AddComponent<MeshRenderer>();
    auto material = std::make_shared<Material>();
    material->Parse("material/fishsoup_pot.mat");
    meshRenderer->SetMaterial(material);
}

void Scene::CreateQuad()
{
    std::vector<Vertex> vertices = {
        {{ -1.0f, -1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f }},
        {{ 1.0f, -1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 0.0f }},
        {{ 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f }},
        {{ -1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f }}
    };

    std::vector<unsigned short> indices = {
        0, 1, 2,
        0, 2, 3
    };

    auto entity = EntityManager::CreateEntity("Quad Entity");
    entity->SetLayer(1);

    auto transform = entity->AddComponent<Transform>();
    transform->SetPosition({ 2, 0, 0 });

    auto meshFilter = entity->AddComponent<MeshFilter>();
    meshFilter->CreateMesh(vertices, indices);

    auto meshRenderer = entity->AddComponent<MeshRenderer>();
    m_fontMaterial = std::make_shared<Material>();
    m_fontMaterial->Parse("material/quad_draw_font.mat");
    meshRenderer->SetMaterial(m_fontMaterial);
}
