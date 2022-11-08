#pragma once

#include <functional>
#include <list>
#include <memory>

#include "Entity.h"

namespace Paimon
{

class Scene
{
public:
    Scene() = default;
    ~Scene() = default;

    static std::shared_ptr<Entity> CreateEntity();
    static std::shared_ptr<Entity> CreateEntity(const std::string &name);

    static void ForEachEntity(const std::function<void(std::shared_ptr<Entity> entity)> &func);

private:
    static std::vector<std::shared_ptr<Entity>> s_entities;

}; // class Scene

} // namespace Paimon
