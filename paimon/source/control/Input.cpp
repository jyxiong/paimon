#include "Input.h"

#include <cassert>

using namespace Paimon;

std::unordered_map<KeyCode, KeyAction> Input::m_keyEvents;
std::unordered_map<MouseButton, MouseButtonAction> Input::m_mouseButtonEvents;
glm::vec2 Input::m_mousePosition{ 0 };
float Input::m_mouseScroll{ 0 };

void Input::RecordKey(KeyCode keyCode, KeyAction keyAction)
{
    m_keyEvents[keyCode] = keyAction;
}

void Input::RecordMouseButton(MouseButton mouseButton, MouseButtonAction mouseButtonAction)
{
    m_mouseButtonEvents[mouseButton] = mouseButtonAction;
}

bool Input::IsKey(KeyCode keyCode)
{
    return m_keyEvents.count(keyCode) > 0;
}

bool Input::IsKeyDown(KeyCode keyCode)
{
    return m_keyEvents.count(keyCode) != 0 && m_keyEvents[keyCode] != KeyAction::Up;
}

bool Input::IsKeyUp(KeyCode keyCode)
{
    return m_keyEvents.count(keyCode) != 0 && m_keyEvents[keyCode] == KeyAction::Up;
}

bool Input::IsMouseButton(MouseButton mouseButton)
{
    return m_mouseButtonEvents.count(mouseButton) > 0;
}

bool Input::IsMouseButtonDown(MouseButton mouseButton)
{
    return m_mouseButtonEvents.count(mouseButton) != 0 && m_mouseButtonEvents[mouseButton] != MouseButtonAction::Up;
}

bool Input::IsMouseButtonUp(MouseButton mouseButton)
{
    return m_mouseButtonEvents.count(mouseButton) != 0 && m_mouseButtonEvents[mouseButton] == MouseButtonAction::Up;
}

void Input::RecordMousePosition(float x, float y)
{
    m_mousePosition.x = x;
    m_mousePosition.y = y;
}

void Input::RecordMouseScroll(float mouseScroll)
{
    m_mouseScroll += mouseScroll;
}

void Input::Update()
{
    for (auto iter = m_keyEvents.begin(); iter != m_keyEvents.end();)
    {
        if (iter->second == KeyAction::Up)
        {
            iter = m_keyEvents.erase(iter);
        } else
        {
            ++iter;
        }
    }

    for (auto iter = m_mouseButtonEvents.begin(); iter != m_mouseButtonEvents.end();)
    {
        if (iter->second == MouseButtonAction::Up)
        {
            iter = m_mouseButtonEvents.erase(iter);
        } else
        {
            ++iter;
        }
    }

    m_mouseScroll = 0;
}
