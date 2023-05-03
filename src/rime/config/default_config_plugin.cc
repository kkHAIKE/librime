//
// Copyright RIME Developers
// Distributed under the BSD License
//
#include <rime/config/config_compiler_impl.h>
#include <rime/config/config_cow_ref.h>
#include <rime/config/plugins.h>

namespace rime {

bool DefaultConfigPlugin::ReviewCompileOutput(
    ConfigCompiler* compiler, an<ConfigResource> resource) {
  return true;
}

bool DefaultConfigPlugin::ReviewLinkOutput(
    ConfigCompiler* compiler, an<ConfigResource> resource) {
  if (!resource->resource_id.ends_with(".schema"))
    return true;
  auto target = Cow(resource, "menu");
  Reference reference{"default", "menu", true};
  if (!IncludeReference{reference}
        .TargetedAt(target).Resolve(compiler)) {
    LOG(ERROR) << "failed to include section " << reference;
    return false;
  }
  return true;
}

}  // namespace rime
