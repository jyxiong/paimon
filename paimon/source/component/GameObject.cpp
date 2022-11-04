#include "GameObject.h"

#include "Component.h"

using namespace Paimon;

GameObject::GameObject(const std::string &name)
    : m_name(name), m_layer(0x01)
{

}

std::shared_ptr<Component> GameObject::AddComponent(const std::string &typeName)
{
    auto t = rttr::type::get_by_name(typeName);
    auto var = t.create();

    auto component = var.get_value<std::shared_ptr<Component>>();
    component->SetGameObject(*this);

    if (m_components.find(typeName) == m_components.end())
    {
        m_components.emplace(typeName, std::vector<std::shared_ptr<Component>>());
    }
    m_components[typeName].emplace_back(component);

    return component;
}

std::vector<std::shared_ptr<Component>> &GameObject::GetComponents(const std::string &typeName)
{
    return m_components[typeName];
}

std::shared_ptr<Component> GameObject::GetComponent(const std::string &typeName)
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
