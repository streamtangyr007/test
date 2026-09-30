add_executable(engine_tests
    test_main.cpp
    test_memory.cpp
    test_ecs.cpp
    test_game_loop.cpp
    test_command_registry.cpp
)
target_link_libraries(engine_tests PRIVATE engine)
add_test(NAME engine_tests COMMAND engine_tests)
