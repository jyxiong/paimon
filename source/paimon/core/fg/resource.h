#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>

#include "paimon/core/fg/pass_node.h"

namespace paimon {
using ResourceId = std::size_t;
using Version = std::size_t;

class ResourceConcept {
public:
  ResourceConcept() = default;
  ResourceConcept(const ResourceConcept &) = delete;
  ResourceConcept(ResourceConcept &&) noexcept = delete;
  virtual ~ResourceConcept() = default;

  ResourceConcept &operator=(const ResourceConcept &) = delete;
  ResourceConcept &operator=(ResourceConcept &&) noexcept = delete;

  virtual void create(void *) = 0;
  virtual void destroy(void *) = 0;

  virtual void preRead(uint32_t flags, void *) = 0;
  virtual void preWrite(uint32_t flags, void *) = 0;

  virtual bool isTransient() const = 0;
};

template <class TResource>
class Resource : public ResourceConcept {
public:
  using Descriptor = typename TResource::Descriptor;

public:
  Resource(const Descriptor &descriptor) : m_descriptor(descriptor) {};

  ~Resource() override = default;

  void create(void *allocator) override {
    m_resource.create(allocator, m_descriptor);
  }

  void destroy(void *allocator) override {
    m_resource.destroy(allocator, m_descriptor);
  }

  void preRead(uint32_t flags, void *context) override {
    m_resource.preRead(context, m_descriptor, flags);
  }

  void preWrite(uint32_t flags, void *context) override {
    m_resource.preWrite(context, m_descriptor, flags);
  }

  bool isTransient() const override { return true; }

  TResource &get() {
    return m_resource;
  }

  Descriptor &get_desc() {
    return m_descriptor;
  }

private:
  Descriptor m_descriptor;
  TResource m_resource;
};

template <class TResource>
class ImportedResource : public Resource<TResource> {
public:
  using Descriptor = typename TResource::Descriptor;

public:
  ImportedResource(const Descriptor &descriptor, const TResource &resource)
    : Resource<TResource>(descriptor) {
    this->m_resource = resource;
  };

  ~ImportedResource() override = default;

  void create(void *allocator) override {
    // No-op for imported resources
  }

  void destroy(void *allocator) override {
    // No-op for imported resources
  }

  void preRead(uint32_t flags, void *context) override {
    // No-op for imported resources
  }

  void preWrite(uint32_t flags, void *context) override {
    // No-op for imported resources
  }

  bool isTransient() const override {
    return false; // Imported resources are not transient
  }
};

class ResourceEntry {
public:
  template <typename T>
  ResourceEntry(ResourceId id, const typename T::Desc &, T &&);

  ResourceEntry() = delete;
  ResourceEntry(const ResourceEntry &) = delete;
  ResourceEntry(ResourceEntry &&) noexcept = default;

  ResourceEntry &operator=(const ResourceEntry &) = delete;
  ResourceEntry &operator=(ResourceEntry &&) noexcept = delete;

  void create(void *allocator);
  void destroy(void *allocator);

  void preRead(uint32_t flags, void *context);
  void preWrite(uint32_t flags, void *context);

  bool isTransient() const { return m_concept->isTransient(); }

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