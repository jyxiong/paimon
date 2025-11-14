#pragma once

#include "paimon/app/event/event.h"
#include "paimon/app/mouse_code.h"

namespace paimon {

class MouseMovedEvent : public Event {
public:
  MouseMovedEvent(float x, float y);

  float getX() const { return m_mouseX; }
  float getY() const { return m_mouseY; }

  std::string toString() const override;

  static EventType getStaticType();
  EventType getEventType() const override;
  const char* getName() const override;
  int getCategoryFlags() const override;

private:
  float m_mouseX, m_mouseY;
};

class MouseScrolledEvent : public Event {
public:
  MouseScrolledEvent(float xOffset, float yOffset);

  float getXOffset() const { return m_xOffset; }
  float getYOffset() const { return m_yOffset; }

  std::string toString() const override;

  static EventType getStaticType();
  EventType getEventType() const override;
  const char* getName() const override;
  int getCategoryFlags() const override;

private:
  float m_xOffset, m_yOffset;
};

class MouseButtonEvent : public Event {
public:
  MouseCode getMouseButton() const { return m_button; }

  int getCategoryFlags() const override { return EventCategoryMouse | EventCategoryMouseButton | EventCategoryInput; }

protected:
  MouseButtonEvent(MouseCode button) : m_button(button) {}

  MouseCode m_button;
};

class MouseButtonPressedEvent : public MouseButtonEvent {
public:
  MouseButtonPressedEvent(MouseCode button);

  std::string toString() const override;

  static EventType getStaticType();
  EventType getEventType() const override;
  const char* getName() const override;
};

class MouseButtonReleasedEvent : public MouseButtonEvent {
public:
  MouseButtonReleasedEvent(MouseCode button);

  std::string toString() const override;

  static EventType getStaticType();
  EventType getEventType() const override;
  const char* getName() const override;
};

} // namespace paimon