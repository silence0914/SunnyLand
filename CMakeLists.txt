# 标题
cmake_minimum_required(VERSION 3.10.0)


set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

project(SunnyLand VERSION 0.1.0 LANGUAGES C CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 设置编译选项
if (MSVC)
    # 使用动态运行时库 (/MD 和 /MDd) 以匹配 spdlog 等第三方库
    add_compile_options(/utf-8)
    add_compile_options(/W4)
else ()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif ()


# 设置编译输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR})

set(TARGET ${PROJECT_NAME}-${CMAKE_SYSTEM_NAME})

# 查找并载入Cmake预设
find_package(SDL3 REQUIRED)
find_package(SDL3_image REQUIRED)
find_package(SDL3_mixer REQUIRED)
find_package(SDL3_ttf REQUIRED)
find_package(glm REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(spdlog REQUIRED)

# 添加可执行文件
add_executable(${TARGET}
        src/main.cpp
        src/engine/core/game_app.cpp
        src/engine/core/game_app.h
        src/engine/core/time.cpp
        src/engine/core/time.h
        src/engine/resource/audio_manager.cpp
        src/engine/resource/audio_manager.h
        src/engine/resource/resource_manager.cpp
        src/engine/resource/resource_manager.h
        src/engine/resource/texture_manager.cpp
        src/engine/resource/texture_manager.h
        src/engine/resource/font_manager.cpp
        src/engine/resource/font_manager.h
        src/engine/utils/math.h
        src/engine/render/Renderer.cpp
        src/engine/render/Renderer.h
        src/engine/render/camera.cpp
        src/engine/render/Camera.h
        src/engine/render/sprite.h
        src/engine/core/Config.cpp
        src/engine/core/Config.h
        src/engine/input/input_manager.cpp
        src/engine/input/input_manager.h
        src/engine/component/component.h
        src/engine/object/game_object.cpp
        src/engine/object/game_object.h
        src/engine/scene/scene.cpp
        src/engine/scene/scene.h
        src/engine/scene/scene_manager.cpp
        src/engine/scene/scene_manager.h
        src/engine/core/context.cpp
        src/engine/core/context.h
        src/engine/component/sprite_component.cpp
        src/engine/component/sprite_component.h
        src/engine/utils/alignment.h
        src/engine/component/transform_component.cpp
        src/engine/component/transform_component.h
        src/engine/log/my_log.h
        src/engine/log/log_manager.h
        src/game/scene/game_scene.cpp
        src/game/scene/game_scene.h
        src/engine/component/parallax_component.cpp
        src/engine/component/parallax_component.h
        src/engine/scene/level_loader.cpp
        src/engine/scene/level_loader.h
        src/engine/utils/action.h
        src/engine/component/tilelayer_component.cpp
        src/engine/component/tilelayer_component.h
)

# 链接库
target_link_libraries(${TARGET}
        ${SDL3_LIBRARIES}
        SDL3_image::SDL3_image
        SDL3_mixer::SDL3_mixer
        SDL3_ttf::SDL3_ttf
        glm::glm
        nlohmann_json::nlohmann_json
        spdlog::spdlog
)