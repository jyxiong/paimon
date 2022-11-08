#include "Entity.h"

#include "Component.h"

using namespace Paimon;

Entity::Entity(const std::string &name)
    : m_name(name)
{

}

std::shared_ptr<Component> Entity::AddComponent(const std::string &typeName)
{
    auto t = rttr::type::get_by_name(typeName);
    auto var = t.create();

    auto component = var.get_value<std::shared_ptr<Component>>();
    component->SetEntity(*this);

    if (m_components.find(typeName) == m_components.end())
    {
        m_components.emplace(typeName, std::vector<std::shared_ptr<Component>>());
    }
    m_components[typeName].emplace_back(component);

    component->Awake();
    return component;
}

std::shared_ptr<Component> Entity::GetComponent(const std::string &typeName)
{
    if (m_components.find(typeName) == m_components.end())
    {
        return nullptr;
    }

    if (m_components[typeName].empty())
    {
        return nullptr;
    }

    return m_components[typeName][0];
}

std::vector<std::shared_ptr<Component>> &Entity::GetComponents(const std::string &typeName)
{
    return m_components[typeName];
}

void Entity::ForEachComponent(const std::function<void(std::shared_ptr<Component>)>& func)
{
    for (const auto &components : m_components)
    {
        for (const auto& component : components.second)
        {
            func(component);
        }
    }
}

std::list<std::shared_ptr<Entity>> EntityManager::s_entities;

std::shared_ptr<Entity> EntityManager::CreateEntity()
{
    return CreateEntity("New Entity");
}

std::shared_ptr<Entity> EntityManager::CreateEntity(const std::string &name)
{
    auto entity = std::make_shared<Entity>(name);

    s_entities.push_back(entity);

    return entity;
}

void EntityManager::ForEachEntity(const std::function<void(std::shared_ptr<Entity>)> &func)
{
    for (auto iter = s_entities.begin(); iter != s_entities.end(); ++iter)
    {
        func(*iter);
    }
}
