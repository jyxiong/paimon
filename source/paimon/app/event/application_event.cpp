#include "paimon/app/event/application_event.h"

#include <sstream>

namespace paimon {

WindowCloseEvent::WindowCloseEvent() {}

std::string WindowCloseEvent::toString() const {
  std::stringstream ss;
  ss << "WindowCloseEvent";
  return ss.str();
}

EventType WindowCloseEvent::getStaticType() { return EventType::WindowClose; }

EventType WindowCloseEvent::getEventType() const { return getStaticType(); }

const char* WindowCloseEvent::getName() const { return "WindowClose"; }

int WindowCloseEvent::getCategoryFlags() const { return EventCategoryApplication; }

WindowResizeEvent::WindowResizeEvent(int32_t width, int32_t height)
    : m_width(width), m_height(height) {}

std::string WindowResizeEvent::toString() const {
  std::stringstream ss;
  ss << "WindowResizeEvent: " << m_width << ", " << m_height;
  return ss.str();
}

EventType WindowResizeEvent::getStaticType() { return EventType::WindowResize; }

EventType WindowResizeEvent::getEventType() const { return getStaticType(); }

const char* WindowResizeEvent::getName() const { return "WindowResize"; }

int WindowResizeEvent::getCategoryFlags() const { return EventCategoryApplication; }

ViewportResizeEvent::ViewportResizeEvent(int32_t width, int32_t height) 
  : m_width(width), m_height(height) {}

std::string ViewportResizeEvent::toString() const {
  std::stringstream ss;
  ss << "ViewportResizeEvent: " << m_width << ", " << m_height;
  return ss.str();
}

EventType ViewportResizeEvent::getStaticType() { return EventType::ViewportResize; }

EventType ViewportResizeEvent::getEventType() const { return getStaticType(); }

const char* ViewportResizeEvent::getName() const { return "ViewportResize"; }

int ViewportResizeEvent::getCategoryFlags() const { return EventCategoryApplication; }

} // namespace paimon