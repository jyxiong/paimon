#pragma once

#include <string>

namespace paimon {

enum class EventType {
  None = 0,
  WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
  AppTick, AppUpdate, AppRender,
  KeyPressed, KeyReleased, KeyTyped,
  MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
};

enum EventCategory {
  None = 0,
  EventCategoryApplication    = 1 << 0,
  EventCategoryInput          = 1 << 1,
  EventCategoryKeyboard       = 1 << 2,
  EventCategoryMouse          = 1 << 3,
  EventCategoryMouseButton    = 1 << 4
};

class Event {
public:
  virtual ~Event() = default;

  bool handled = false;

  virtual EventType getEventType() const = 0;
  virtual const char* getName() const = 0;
  virtual int getCategoryFlags() const = 0;
  virtual std::string toString() const { return getName(); }

  bool isInCategory(EventCategory category);
};

class EventDispatcher {
public:
  EventDispatcher(Event& event) : m_event(event) {}

  template<typename T, typename F>
  bool dispatch(const F& func) {
    if (m_event.getEventType() == T::getStaticType()) {
      m_event.handled = func(static_cast<T&>(m_event));
      return true;
    }
    return false;
  }

private:
  Event& m_event;
};

} // namespace paimon