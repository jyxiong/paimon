#pragma once

#include <unordered_map>

#include "glm/glm.hpp"

#include "KeyCode.h"

namespace Paimon
{

class Input
{
public:
    static void RecordKey(KeyCode keyCode, KeyAction keyAction);
    static bool IsKey(KeyCode keyCode);
    static bool IsKeyDown(KeyCode keyCode);
    static bool IsKeyUp(KeyCode keyCode);

    static void RecordMouseButton(MouseButton mouseButton, MouseButtonAction mouseButtonAction);
    static bool IsMouseButton(MouseButton mouseButton);
    static bool IsMouseButtonDown(MouseButton mouseButton);
    static bool IsMouseButtonUp(MouseButton mouseButton);

    static void RecordMousePosition(float x, float y);
    static glm::vec2 GetMousePosition() { return m_mousePosition; };

    static void RecordMouseScroll(float mouseScroll);
    static float GetMouseScroll() { return m_mouseScroll; }

    static void Update();

private:
    static std::unordered_map<KeyCode, KeyAction> m_keyEvents;
    static std::unordered_map<MouseButton, MouseButtonAction> m_mouseButtonEvents;

    static glm::vec2 m_mousePosition;
    static float m_mouseScroll;

}; // class Input

} // namespace Paimon
