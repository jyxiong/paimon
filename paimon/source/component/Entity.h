#pragma once

#include <functional>
#include <list>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "rttr/registration"

namespace Paimon
{
class Component;
class Entity
{
public:
    Entity() = default;
    explicit Entity(const std::string &name);
    ~Entity() = default;

    void SetName(const std::string &name) { m_name = name; }
    std::string &GetName() { return m_name; }

    void SetLayer(unsigned char layer) { m_layer = layer; }
    [[nodiscard]] unsigned char GetLayer() const { return m_layer; }

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

        component->Awake();
        return component;
    }

    std::vector<std::shared_ptr<Component>> &GetComponents(const std::string &typeName);

    std::shared_ptr<Component> GetComponent(const std::string &typeName);

    template<typename T>
    std::shared_ptr<T> GetComponent()
    {
        auto t = rttr::type::get<T>();
        auto typeName = t.get_name().to_string();

        std::vector<std::shared_ptr<Component>> components;
        if (m_components.find(typeName) != m_components.end())
        {
            components = m_components[typeName];
        }

        if (components.empty())
        {
            return nullptr;
        }

        return std::dynamic_pointer_cast<T>(components[0]);
    }

    void ForEachComponent(const std::function<void(std::shared_ptr<Component> component)>& func);

private:
    std::string m_name;

    unsigned char m_layer{ 0x01 };

    std::unordered_map<std::string, std::vector<std::shared_ptr<Component>>> m_components;

}; // class

class EntityManager
{
public:
    EntityManager() = default;
    ~EntityManager() = default;

    static std::shared_ptr<Entity> CreateEntity();
    static std::shared_ptr<Entity> CreateEntity(const std::string &name);

    static void ForEachEntity(const std::function<void(std::shared_ptr<Entity> entity)> &func);

private:
    static std::list<std::shared_ptr<Entity>> s_entities;
};

} // namespace Paimon
