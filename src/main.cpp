#include <iostream>
#include <spdlog/spdlog.h>
#include "engine/core/game_app.h"
int main() {
    spdlog::set_level(spdlog::level::trace);

    engine::core::GameApp game;
    game.Run();
    return 0;
}
