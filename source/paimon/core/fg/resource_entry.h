#pragma once

#include "paimon/core/fg/resource.h"

namespace paimon {

class ResourceEntry {
public:
  ResourceEntry(ResourceId id, std::unique_ptr<ResourceConcept> &&resource)
      : m_id(id), m_version(0), m_concept(std::move(resource)) {}

  ResourceEntry() = delete;
  ResourceEntry(const ResourceEntry &) = delete;
  ResourceEntry(ResourceEntry &&) noexcept = default;

  ResourceEntry &operator=(const ResourceEntry &) = delete;
  ResourceEntry &operator=(ResourceEntry &&) noexcept = delete;

  void create(void *allocator) { m_concept->create(allocator); }
  void destroy(void *allocator) { m_concept->destroy(allocator); }

  void preRead(uint32_t flags, void *context) {
    m_concept->preRead(flags, context);
  }
  void preWrite(uint32_t flags, void *context) {
    m_concept->preWrite(flags, context);
  }

  bool isTransient() const { return m_concept->isTransient(); }

  void incrementVersion() { ++m_version; }

  ResourceId getId() const { return m_id; }
  Version getVersion() const { return m_version; }
  PassNode *getProducer() const { return m_producer; }
  PassNode *getLast() const { return m_last; }

  void setProducer(PassNode *producer) { m_producer = producer; }
  void setLast(PassNode *last) { m_last = last; }

  template <class TResource>
  TResource &get() {
    return dynamic_cast<Resource<TResource>>(*m_concept).get();
  }

  template <class TResource>
  typename TResource::Descriptor &get_desc() {
    return dynamic_cast<Resource<TResource>>(*m_concept).get_desc();
  }

private:
  ResourceId m_id;
  Version m_version;
  std::unique_ptr<ResourceConcept> m_concept;

  PassNode *m_producer{nullptr};
  PassNode *m_last{nullptr};
};

} // namespace paimon