#include <iostream>
#include "engine/log/log_manager.h"
#include "engine/core/game_app.h"

int main() {
    mylog::LogManager log_manager{"SunnyLand"};
    spdlog::set_level(spdlog::level::trace);

    engine::core::GameApp game;
    game.Run();
    return 0;
}
