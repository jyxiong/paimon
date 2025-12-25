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

WindowFocusEvent::WindowFocusEvent() {}

std::string WindowFocusEvent::toString() const {
  std::stringstream ss;
  ss << "WindowFocusEvent";
  return ss.str();
}

EventType WindowFocusEvent::getStaticType() { return EventType::WindowFocus; }

EventType WindowFocusEvent::getEventType() const { return getStaticType(); }

const char* WindowFocusEvent::getName() const { return "WindowFocus"; }

int WindowFocusEvent::getCategoryFlags() const { return EventCategoryApplication; }

WindowLostFocusEvent::WindowLostFocusEvent() {}

std::string WindowLostFocusEvent::toString() const {
  std::stringstream ss;
  ss << "WindowLostFocusEvent";
  return ss.str();
}

EventType WindowLostFocusEvent::getStaticType() { return EventType::WindowLostFocus; }

EventType WindowLostFocusEvent::getEventType() const { return getStaticType(); }

const char* WindowLostFocusEvent::getName() const { return "WindowLostFocus"; }

int WindowLostFocusEvent::getCategoryFlags() const { return EventCategoryApplication; }

WindowMovedEvent::WindowMovedEvent(int x, int y) : m_x(x), m_y(y) {}

std::string WindowMovedEvent::toString() const {
  std::stringstream ss;
  ss << "WindowMovedEvent: " << m_x << ", " << m_y;
  return ss.str();
}

EventType WindowMovedEvent::getStaticType() { return EventType::WindowMoved; }

EventType WindowMovedEvent::getEventType() const { return getStaticType(); }

const char* WindowMovedEvent::getName() const { return "WindowMoved"; }

int WindowMovedEvent::getCategoryFlags() const { return EventCategoryApplication; }

AppTickEvent::AppTickEvent() {}

std::string AppTickEvent::toString() const {
  std::stringstream ss;
  ss << "AppTickEvent";
  return ss.str();
}

EventType AppTickEvent::getStaticType() { return EventType::AppTick; }

EventType AppTickEvent::getEventType() const { return getStaticType(); }

const char* AppTickEvent::getName() const { return "AppTick"; }

int AppTickEvent::getCategoryFlags() const { return EventCategoryApplication; }

AppUpdateEvent::AppUpdateEvent() {}

std::string AppUpdateEvent::toString() const {
  std::stringstream ss;
  ss << "AppUpdateEvent";
  return ss.str();
}

EventType AppUpdateEvent::getStaticType() { return EventType::AppUpdate; }

EventType AppUpdateEvent::getEventType() const { return getStaticType(); }

const char* AppUpdateEvent::getName() const { return "AppUpdate"; }

int AppUpdateEvent::getCategoryFlags() const { return EventCategoryApplication; }

AppRenderEvent::AppRenderEvent() {}

std::string AppRenderEvent::toString() const {
  std::stringstream ss;
  ss << "AppRenderEvent";
  return ss.str();
}

EventType AppRenderEvent::getStaticType() { return EventType::AppRender; }

EventType AppRenderEvent::getEventType() const { return getStaticType(); }

const char* AppRenderEvent::getName() const { return "AppRender"; }

int AppRenderEvent::getCategoryFlags() const { return EventCategoryApplication; }

} // namespace paimon