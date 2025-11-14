#pragma once

#include "paimon/app/event/event.h"
#include "paimon/app/key_code.h"

namespace paimon {

class KeyEvent : public Event {
public:
  KeyCode getKeyCode() const { return m_keyCode; }

  int getCategoryFlags() const override { return EventCategoryKeyboard | EventCategoryInput; }

protected:
  KeyEvent(KeyCode keycode) : m_keyCode(keycode) {}

  KeyCode m_keyCode;
};

class KeyPressedEvent : public KeyEvent {
public:
  KeyPressedEvent(KeyCode keycode, int repeatCount);

  int getRepeatCount() const { return m_repeatCount; }

  std::string toString() const override;

  static EventType getStaticType();
  EventType getEventType() const override;
  const char* getName() const override;

private:
  int m_repeatCount;
};

class KeyReleasedEvent : public KeyEvent {
public:
  KeyReleasedEvent(KeyCode keycode);

  std::string toString() const override;

  static EventType getStaticType();
  EventType getEventType() const override;
  const char* getName() const override;
};

class KeyTypedEvent : public KeyEvent {
public:
  KeyTypedEvent(KeyCode keycode);

  std::string toString() const override;

  static EventType getStaticType();
  EventType getEventType() const override;
  const char* getName() const override;
};

} // namespace paimon