#include "paimon/app/event/mouse_event.h"

#include <sstream>

namespace paimon {

MouseMovedEvent::MouseMovedEvent(float x, float y) : m_mouseX(x), m_mouseY(y) {}

std::string MouseMovedEvent::toString() const {
  std::stringstream ss;
  ss << "MouseMovedEvent: " << m_mouseX << ", " << m_mouseY;
  return ss.str();
}

EventType MouseMovedEvent::getStaticType() { return EventType::MouseMoved; }

EventType MouseMovedEvent::getEventType() const { return getStaticType(); }

const char* MouseMovedEvent::getName() const { return "MouseMoved"; }

int MouseMovedEvent::getCategoryFlags() const { return EventCategoryMouse | EventCategoryInput; }

MouseScrolledEvent::MouseScrolledEvent(float xOffset, float yOffset)
    : m_xOffset(xOffset), m_yOffset(yOffset) {}

std::string MouseScrolledEvent::toString() const {
  std::stringstream ss;
  ss << "MouseScrolledEvent: " << getXOffset() << ", " << getYOffset();
  return ss.str();
}

EventType MouseScrolledEvent::getStaticType() { return EventType::MouseScrolled; }

EventType MouseScrolledEvent::getEventType() const { return getStaticType(); }

const char* MouseScrolledEvent::getName() const { return "MouseScrolled"; }

int MouseScrolledEvent::getCategoryFlags() const { return EventCategoryMouse | EventCategoryInput; }

MouseButtonPressedEvent::MouseButtonPressedEvent(MouseCode button) : MouseButtonEvent(button) {}

std::string MouseButtonPressedEvent::toString() const {
  std::stringstream ss;
  ss << "MouseButtonPressedEvent: " << static_cast<int>(m_button);
  return ss.str();
}

EventType MouseButtonPressedEvent::getStaticType() { return EventType::MouseButtonPressed; }

EventType MouseButtonPressedEvent::getEventType() const { return getStaticType(); }

const char* MouseButtonPressedEvent::getName() const { return "MouseButtonPressed"; }

MouseButtonReleasedEvent::MouseButtonReleasedEvent(MouseCode button) : MouseButtonEvent(button) {}

std::string MouseButtonReleasedEvent::toString() const {
  std::stringstream ss;
  ss << "MouseButtonReleasedEvent: " << static_cast<int>(m_button);
  return ss.str();
}

EventType MouseButtonReleasedEvent::getStaticType() { return EventType::MouseButtonReleased; }

EventType MouseButtonReleasedEvent::getEventType() const { return getStaticType(); }

const char* MouseButtonReleasedEvent::getName() const { return "MouseButtonReleased"; }

} // namespace paimon