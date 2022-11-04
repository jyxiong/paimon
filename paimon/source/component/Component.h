#pragma once

#include <memory>

namespace Paimon
{

class GameObject;

class Component : public std::enable_shared_from_this<Component>
{
public:
    explicit Component(GameObject &gameObject);
    virtual ~Component() = default;

    void SetGameObject(GameObject &gameObject);
    GameObject &GetGameObject() { return m_gameObject; }

private:
    GameObject &m_gameObject;

}; // class Component

} // namespace Paimon
