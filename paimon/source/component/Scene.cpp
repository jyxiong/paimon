#include "Scene.h"

using namespace Paimon;

std::vector<std::shared_ptr<Entity>> Scene::s_entities;

std::shared_ptr<Entity> Scene::CreateEntity()
{
    return CreateEntity("New Entity");
}

std::shared_ptr<Entity> Scene::CreateEntity(const std::string &name)
{
    auto entity = std::make_shared<Entity>(name);

    s_entities.emplace_back(entity);

    return entity;
}

void Scene::ForEachEntity(const std::function<void(std::shared_ptr<Entity> entity)> &func)
{
    for (auto iter = s_entities.begin(); iter != s_entities.end(); ++iter)
    {
        func(*iter);
    }
}

