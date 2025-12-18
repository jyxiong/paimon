#pragma once

namespace paimon::ecs {

class System {
public:
  virtual ~System() = default;
  
  virtual void update(float deltaTime) = 0;
};
}