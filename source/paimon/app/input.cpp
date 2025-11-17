#include "paimon/app/input.h"
#include "paimon/app/application.h"

namespace paimon {

bool Input::isKeyPressed(KeyCode key) {
    auto window = Application::getInstance().getWindow();
    if (!window) return false;
    return window->isKeyPressed(key);
}

bool Input::isMouseButtonPressed(MouseCode button) {
    auto window = Application::getInstance().getWindow();
    if (!window) return false;
    return window->isMouseButtonPressed(button);
}

std::pair<double, double> Input::getMousePosition() {
    auto window = Application::getInstance().getWindow();
    if (!window) return {0.0, 0.0};
    return window->getMousePosition();
}

} // namespace paimon