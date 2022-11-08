#include "Component.h"

#include "Entity.h"

using namespace Paimon;

Component::Component(Entity &entity)
    : m_entity(entity)
{

}

void Component::SetEntity(Entity &entity)
{
    m_entity = entity;
}

void Component::Awake()
{

}

void Component::Update()
{

}
