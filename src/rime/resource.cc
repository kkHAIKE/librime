//
// Copyright RIME Developers
// Distributed under the BSD License
//

#include <filesystem>
#include <rime/resource.h>

namespace rime {

string ResourceResolver::ToResourceId(const string& file_path) const {
  string path_string = std::filesystem::path(file_path).generic_string();
  bool has_prefix = path_string.starts_with(type_.prefix);
  bool has_suffix = path_string.ends_with(type_.suffix);
  size_t start = (has_prefix ? type_.prefix.length() : 0);
  size_t end = path_string.length() - (has_suffix ? type_.suffix.length() : 0);
  return path_string.substr(start, end);
}

string ResourceResolver::ToFilePath(const string& resource_id) const {
  std::filesystem::path file_path(resource_id);
  bool missing_prefix = !file_path.has_parent_path() &&
      !resource_id.starts_with(type_.prefix);
  bool missing_suffix = !resource_id.ends_with(type_.suffix);
  return (missing_prefix ? type_.prefix : "") + resource_id +
      (missing_suffix ? type_.suffix : "");
}

std::filesystem::path ResourceResolver::ResolvePath(const string& resource_id) {
  return std::filesystem::absolute(root_path_) /
      std::filesystem::path(type_.prefix + resource_id + type_.suffix);
}

std::filesystem::path
FallbackResourceResolver::ResolvePath(const string& resource_id) {
  auto default_path = ResourceResolver::ResolvePath(resource_id);
  if (!std::filesystem::exists(default_path)) {
    auto fallback_path = std::filesystem::absolute(fallback_root_path_) /
        std::filesystem::path(type_.prefix + resource_id + type_.suffix);
    if (std::filesystem::exists(fallback_path)) {
      return fallback_path;
    }
  }
  return default_path;
}

}  // namespace rime
