#pragma once

#include <utility>

#include "paimon/app/key_code.h"
#include "paimon/app/mouse_code.h"

namespace paimon {

class Input {
public:
    static bool isKeyPressed(KeyCode key);
    static bool isMouseButtonPressed(MouseCode button);
    static std::pair<double, double> getMousePosition();
};

} // namespace paimon