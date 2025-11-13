# ECS (Entity Component System)

这是基于 EnTT 库封装的 ECS 系统，提供了简洁的 API 来管理游戏对象和组件。

## 核心类

### World
`World` 是 ECS 的核心容器，管理所有实体和组件。

```cpp
#include "paimon/core/ecs/ecs.h"

using namespace paimon::ecs;

// 创建世界
World world;

// 创建实体
Entity entity = world.createEntity();

// 销毁实体
world.destroyEntity(entity);

// 清空所有实体
world.clear();

// 检查实体是否有效
bool valid = world.valid(entity.getHandle());

// 遍历所有具有特定组件的实体
auto view = world.view<TransformComponent, MeshComponent>();
for (auto [entity, transform, mesh] : view.each()) {
    // 处理实体
}
```

### Entity
`Entity` 是游戏对象的封装，提供组件管理功能。

```cpp
// 添加组件
auto& transform = entity.addComponent<TransformComponent>();
transform.position = glm::vec3(0.0f, 1.0f, 0.0f);

// 获取组件
auto& transform = entity.getComponent<TransformComponent>();

// 尝试获取组件（如果不存在返回 nullptr）
if (auto* transform = entity.tryGetComponent<TransformComponent>()) {
    // 使用组件
}

// 检查是否有组件
if (entity.hasComponent<TransformComponent>()) {
    // ...
}

// 检查是否有所有指定的组件
if (entity.hasAllComponents<TransformComponent, MeshComponent>()) {
    // ...
}

// 检查是否有任意指定的组件
if (entity.hasAnyComponent<TransformComponent, MeshComponent>()) {
    // ...
}

// 移除组件
entity.removeComponent<TransformComponent>();

// 替换组件
entity.replaceComponent<TransformComponent>(/* 新参数 */);

// 获取或添加组件（如果不存在则添加）
auto& transform = entity.getOrAddComponent<TransformComponent>();

// 检查实体是否有效
if (entity.isValid() || entity) {
    // 实体有效
}
```

## 预定义组件

### IDComponent
唯一标识符组件
```cpp
struct IDComponent {
    uint64_t id = 0;
};
```

### TagComponent
标签/名称组件
```cpp
struct TagComponent {
    std::string tag;
};
```

### TransformComponent
变换组件（位置、旋转、缩放）
```cpp
struct TransformComponent {
    glm::vec3 position = glm::vec3(0.0f);
    glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    glm::vec3 scale = glm::vec3(1.0f);
    
    glm::mat4 getTransformMatrix() const;
};
```

### HierarchyComponent
层级关系组件
```cpp
struct HierarchyComponent {
    entt::entity parent = entt::null;
    std::vector<entt::entity> children;
};
```

### CameraComponent
相机组件
```cpp
struct CameraComponent {
    float fov = 45.0f;
    float nearPlane = 0.1f;
    float farPlane = 1000.0f;
    bool isPrimary = true;
    
    glm::mat4 getProjectionMatrix(float aspectRatio) const;
};
```

### MeshComponent
网格组件
```cpp
struct MeshComponent {
    uint32_t meshHandle = 0;
    uint32_t materialHandle = 0;
};
```

### LightComponent
光源组件
```cpp
struct LightComponent {
    enum class Type {
        Directional,
        Point,
        Spot
    } type = Type::Directional;
    
    glm::vec3 color = glm::vec3(1.0f);
    float intensity = 1.0f;
    float range = 10.0f;
    float innerConeAngle = 15.0f;
    float outerConeAngle = 30.0f;
};
```

## 使用示例

```cpp
#include "paimon/core/ecs/ecs.h"

using namespace paimon::ecs;

int main() {
    // 创建世界
    World world;
    
    // 创建一个带有变换和网格的实体
    Entity entity = world.createEntity();
    entity.addComponent<IDComponent>().id = 1;
    entity.addComponent<TagComponent>().tag = "Player";
    
    auto& transform = entity.addComponent<TransformComponent>();
    transform.position = glm::vec3(0.0f, 0.0f, 0.0f);
    transform.scale = glm::vec3(1.0f);
    
    auto& mesh = entity.addComponent<MeshComponent>();
    mesh.meshHandle = 0;
    mesh.materialHandle = 0;
    
    // 创建相机实体
    Entity camera = world.createEntity();
    camera.addComponent<TagComponent>().tag = "MainCamera";
    
    auto& cameraTransform = camera.addComponent<TransformComponent>();
    cameraTransform.position = glm::vec3(0.0f, 5.0f, 10.0f);
    
    auto& cameraComp = camera.addComponent<CameraComponent>();
    cameraComp.fov = 60.0f;
    cameraComp.isPrimary = true;
    
    // 创建光源实体
    Entity light = world.createEntity();
    light.addComponent<TagComponent>().tag = "DirectionalLight";
    
    auto& lightTransform = light.addComponent<TransformComponent>();
    lightTransform.position = glm::vec3(0.0f, 10.0f, 0.0f);
    
    auto& lightComp = light.addComponent<LightComponent>();
    lightComp.type = LightComponent::Type::Directional;
    lightComp.color = glm::vec3(1.0f, 1.0f, 1.0f);
    lightComp.intensity = 1.0f;
    
    // 系统：更新所有带变换和网格的实体
    auto view = world.view<TransformComponent, MeshComponent>();
    for (auto [entity, transform, mesh] : view.each()) {
        glm::mat4 transformMatrix = transform.getTransformMatrix();
        // 渲染网格...
    }
    
    // 系统：处理相机
    auto cameraView = world.view<CameraComponent, TransformComponent>();
    for (auto [entity, camera, transform] : cameraView.each()) {
        if (camera.isPrimary) {
            glm::mat4 projection = camera.getProjectionMatrix(16.0f / 9.0f);
            // 设置相机矩阵...
        }
    }
    
    return 0;
}
```

## 自定义组件

你可以轻松创建自定义组件：

```cpp
struct VelocityComponent {
    glm::vec3 velocity = glm::vec3(0.0f);
};

struct HealthComponent {
    float current = 100.0f;
    float max = 100.0f;
};

// 使用
entity.addComponent<VelocityComponent>().velocity = glm::vec3(1.0f, 0.0f, 0.0f);
entity.addComponent<HealthComponent>().max = 200.0f;
```

## 注意事项

1. 组件应该是简单的数据结构（POD），不包含复杂的逻辑
2. 所有游戏逻辑应该通过系统（System）来实现，系统遍历具有特定组件的实体
3. Entity 对象是轻量级的，可以按值传递
4. 销毁实体时，所有关联的组件会自动清理
5. 使用 `tryGetComponent` 而不是 `hasComponent + getComponent` 可以提高性能
