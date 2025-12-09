#include "paimon/app/event/key_event.h"

#include <sstream>

namespace paimon {

KeyPressedEvent::KeyPressedEvent(KeyCode keycode, int repeatCount)
    : KeyEvent(keycode), m_repeatCount(repeatCount) {}

std::string KeyPressedEvent::toString() const {
  std::stringstream ss;
  ss << "KeyPressedEvent: " << static_cast<int>(m_keyCode) << " (" << m_repeatCount << " repeats)";
  return ss.str();
}

EventType KeyPressedEvent::getStaticType() { return EventType::KeyPressed; }

EventType KeyPressedEvent::getEventType() const { return getStaticType(); }

const char* KeyPressedEvent::getName() const { return "KeyPressed"; }

KeyReleasedEvent::KeyReleasedEvent(KeyCode keycode) : KeyEvent(keycode) {}

std::string KeyReleasedEvent::toString() const {
  std::stringstream ss;
  ss << "KeyReleasedEvent: " << static_cast<int>(m_keyCode);
  return ss.str();
}

EventType KeyReleasedEvent::getStaticType() { return EventType::KeyReleased; }

EventType KeyReleasedEvent::getEventType() const { return getStaticType(); }

const char* KeyReleasedEvent::getName() const { return "KeyReleased"; }

KeyTypedEvent::KeyTypedEvent(KeyCode keycode) : KeyEvent(keycode) {}

std::string KeyTypedEvent::toString() const {
  std::stringstream ss;
  ss << "KeyTypedEvent: " << static_cast<int>(m_keyCode);
  return ss.str();
}

EventType KeyTypedEvent::getStaticType() { return EventType::KeyTyped; }

EventType KeyTypedEvent::getEventType() const { return getStaticType(); }

const char* KeyTypedEvent::getName() const { return "KeyTyped"; }

} // namespace paimon