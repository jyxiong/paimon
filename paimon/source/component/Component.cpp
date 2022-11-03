#include "Component.h"

#include "GameObject.h"

using namespace Paimon;

Component::Component(GameObject &gameObject)
    : m_gameObject(gameObject)
{

}

void Component::SetGameObject(GameObject &gameObject)
{
    m_gameObject = gameObject;
}
