#include "paimon/app/event/event.h"

namespace paimon {

bool Event::isInCategory(EventCategory category) {
  return getCategoryFlags() & category;
}

} // namespace paimon