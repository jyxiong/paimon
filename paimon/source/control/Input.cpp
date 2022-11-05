#include "Input.h"

#include <cassert>

using namespace Paimon;

std::unordered_map<KeyCode, KeyAction> Input::m_keyEvents;

void Input::RecordKey(KeyCode keyCode, KeyAction keyAction)
{
    // if (keyCode < 0)
    // {
    //     assert(false);
    //     return;
    // }
    m_keyEvents[keyCode] = keyAction;
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
}

bool Input::IsKey(KeyCode keyCode)
{
    return m_keyEvents.count(keyCode) > 0;
}

bool Input::IsKeyDown(KeyCode keyCode)
{
    if (m_keyEvents.count(keyCode) == 0)
    {
        return false;
    }

    return m_keyEvents[keyCode] != KeyAction::Up;
}

bool Input::IsKeyUp(KeyCode keyCode)
{
    if (m_keyEvents.count(keyCode) == 0)
    {
        return false;
    }

    return m_keyEvents[keyCode] == KeyAction::Up;
}

