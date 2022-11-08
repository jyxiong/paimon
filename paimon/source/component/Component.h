#pragma once

#include <memory>

namespace Paimon
{

class Entity;

class Component
{
public:
    explicit Component(Entity &entity);
    virtual ~Component() = default;

    void SetEntity(Entity &entity);
    Entity &GetEntity() { return m_entity; }

    virtual void Awake();
    virtual void Update();

private:
    Entity &m_entity;

}; // class Component

} // namespace Paimon
