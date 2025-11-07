#include "paimon/asset/asset.h"

using namespace paimon;

AssetMetadata::AssetMetadata(
  const std::filesystem::path &path, AssetType assetType
)
  : filePath(path), type(assetType) {
  if (std::filesystem::exists(path)) {
    auto ftime = std::filesystem::last_write_time(path);
    auto sctp =
      std::chrono::time_point_cast<std::chrono::system_clock::duration>(
        ftime - std::filesystem::file_time_type::clock::now() +
        std::chrono::system_clock::now()
      );
    lastModified = sctp;
  }
}

Asset::Asset(const AssetMetadata &metadata) : m_metadata(metadata) {}

bool Asset::reload() {
  unload();
  return load();
}

const AssetMetadata &Asset::getMetadata() const { return m_metadata; }

const std::filesystem::path &Asset::getPath() const {
  return m_metadata.filePath;
}

AssetType Asset::getType() const { return m_metadata.type; }

size_t Asset::getMemorySize() const { return m_metadata.memorySize; }
