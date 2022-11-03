#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "rttr/registration"

namespace Paimon
{
class Component;
class GameObject
{
public:
    GameObject() = default;
    explicit GameObject(const std::string &name);
    ~GameObject() = default;

    void SetName(const std::string &name) { m_name = name; }
    std::string &GetName() { return m_name; }

    std::shared_ptr<Component> AddComponent(const std::string &typeName);

    template<typename T>
    std::shared_ptr<T> AddComponent()
    {
        auto component = std::make_shared<T>(*this);

        auto t = rttr::type::get(*component.get());
        auto typeName = t.get_name().to_string();

        if (m_components.find(typeName) == m_components.end())
        {
            m_components.emplace(typeName, std::vector<std::shared_ptr<Component>>());
        }
        m_components[typeName].emplace_back(component);

        return component;
    }

    std::vector<std::shared_ptr<Component>> &GetComponents(const std::string &typeName);
    std::shared_ptr<Component> GetComponent(const std::string &typeName);

private:
    std::string m_name;

    std::unordered_map<std::string, std::vector<std::shared_ptr<Component>>> m_components;

}; // class

} // namespace Paimon
