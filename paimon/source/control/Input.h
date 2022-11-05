#pragma once

#include <unordered_map>

#include "KeyCode.h"

namespace Paimon
{

class Input
{
public:
    static void RecordKey(KeyCode keyCode, KeyAction keyAction);

    static void Update();

    static bool IsKey(KeyCode keyCode);

    static bool IsKeyDown(KeyCode keyCode);

    static bool IsKeyUp(KeyCode keyCode);

private:
    static std::unordered_map<KeyCode, KeyAction> m_keyEvents;

}; // class Input

} // namespace Paimon
