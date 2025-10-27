#pragma once

#include <utility>

namespace paimon {
class FrameGraphResources;

class PassConcept {
public:
  PassConcept() = default;
  PassConcept(const PassConcept &) = delete;
  PassConcept(PassConcept &&) noexcept = delete;
  virtual ~PassConcept() = default;

  PassConcept &operator=(const PassConcept &) = delete;
  PassConcept &operator=(PassConcept &&) noexcept = delete;

  virtual void execute(FrameGraphResources &, void *) const = 0;
};

template <class TData, class TExecutor>
class Pass : public PassConcept {
public:
  Pass(TExecutor &&executor) : m_executor(std::forward<TExecutor>(executor)) {}

  ~Pass() override = default;

  void execute(FrameGraphResources &resources, void *context) const override {
    m_executor(resources, context);
  }

  TData &get_data() { return m_data; }
  const TData &get_data() const { return m_data; }

private:
  TData m_data;
  TExecutor m_executor;
};
} // namespace paimon