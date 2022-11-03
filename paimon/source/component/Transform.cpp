#include "Transform.h"

#include "rttr/registration"

using namespace Paimon;

RTTR_REGISTRATION//注册反射
{
    rttr::registration::class_<Transform>("Transform")
        .constructor<GameObject>()(rttr::policy::ctor::as_raw_ptr)
        .property("position", &Transform::GetPosition, &Transform::SetPosition)
        .property("rotation", &Transform::GetRotation, &Transform::SetRotation)
        .property("scale", &Transform::GetScale, &Transform::SetScale);
}

Transform::Transform(GameObject &gameObject)
    : Component(gameObject)
{

}
