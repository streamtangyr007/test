add_library(engine STATIC
    src/core/log.cpp
    src/core/game_loop.cpp
    src/tools/command_registry.cpp
)
target_include_directories(engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(engine PUBLIC cxx_std_20)
