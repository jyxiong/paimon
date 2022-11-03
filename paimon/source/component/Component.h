#pragma once

namespace Paimon
{

class GameObject;

class Component
{
public:
    explicit Component(GameObject &gameObject);
    virtual ~Component() = default;

    GameObject &GetGameObject() { return m_gameObject; }

    void SetGameObject(GameObject &gameObject);
private:
    GameObject &m_gameObject;

}; // class Component

} // namespace Paimon
