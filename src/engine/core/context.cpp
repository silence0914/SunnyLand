//
// Created by wangm on 26-10-14.
//

#include "context.h"

#include <spdlog/spdlog.h>

namespace engine {
namespace core {
Context::Context(engine::render::Camera& camera,
                 engine::render::Renderer& renderer,
                 engine::input::InputManager& input_manager,
                 engine::resource::ResourceManager& resource_manager,
                 engine::physics::PhysicsEngine& physics_engine)
    : camera_(camera),
      renderer_(renderer),
      input_manager_(input_manager),
      resource_manager_(resource_manager),
      physics_engine_(physics_engine) {
    spdlog::debug("create Context completion");
}
} // core
} // engine
