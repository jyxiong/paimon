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

class ViewportResizeEvent : public Event {
public:
  ViewportResizeEvent(int32_t width, int32_t height);

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

} // namespace paimon