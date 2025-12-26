#pragma once

#include <cstdint>

#include "paimon/app/event/event.h"

namespace paimon {

class WindowCloseEvent : public Event {
public:
  WindowCloseEvent();

  std::string toString() const override;

  static EventType getStaticType();
  EventType getEventType() const override;
  const char* getName() const override;
  int getCategoryFlags() const override;
};

class WindowResizeEvent : public Event {
public:
  WindowResizeEvent(int32_t width, int32_t height);

  int32_t getWidth() const { return m_width; }
  int32_t getHeight() const { return m_height; }

  std::string toString() const override;

  static EventType getStaticType();
  EventType getEventType() const override;
  const char* getName() const override;
  int getCategoryFlags() const override;

private:
  int32_t m_width, m_height;
};

class WindowFocusEvent : public Event {
public:
  WindowFocusEvent();

  std::string toString() const override;

  static EventType getStaticType();
  EventType getEventType() const override;
  const char* getName() const override;
  int getCategoryFlags() const override;
};

class WindowLostFocusEvent : public Event {
public:
  WindowLostFocusEvent();

  std::string toString() const override;

  static EventType getStaticType();
  EventType getEventType() const override;
  const char* getName() const override;
  int getCategoryFlags() const override;
};

class WindowMovedEvent : public Event {
public:
  WindowMovedEvent(int x, int y);

  int getX() const { return m_x; }
  int getY() const { return m_y; }

  std::string toString() const override;

  static EventType getStaticType();
  EventType getEventType() const override;
  const char* getName() const override;
  int getCategoryFlags() const override;

private:
  int m_x, m_y;
};

class AppTickEvent : public Event {
public:
  AppTickEvent();

  std::string toString() const override;

  static EventType getStaticType();
  EventType getEventType() const override;
  const char* getName() const override;
  int getCategoryFlags() const override;
};

class AppUpdateEvent : public Event {
public:
  AppUpdateEvent();

  std::string toString() const override;

  static EventType getStaticType();
  EventType getEventType() const override;
  const char* getName() const override;
  int getCategoryFlags() const override;
};

class AppRenderEvent : public Event {
public:
  AppRenderEvent();

  std::string toString() const override;

  static EventType getStaticType();
  EventType getEventType() const override;
  const char* getName() const override;
  int getCategoryFlags() const override;
};

} // namespace paimon