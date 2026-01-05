cmake_minimum_required(VERSION 3.10...4.1)
project(Byteman VERSION 0.1.0 LANGUAGES C)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(COMMANDS_DIR "src/commands")
set(CLI_DIR "src/cli")

# Commands lib
add_library(commands STATIC ${COMMANDS_DIR}/get.c ${COMMANDS_DIR}/login.c ${COMMANDS_DIR}/signup.c)
target_include_directories(commands PUBLIC include)

# cli lib
add_library(cli STATIC ${CLI_DIR}/cli_parse.c ${CLI_DIR}/command_dispatch.c)
target_include_directories(cli PUBLIC include)
target_link_libraries(cli PUBLIC commands)

# main exe
add_executable(Byteman src/main.c src/utils.c)
target_link_libraries(Byteman PUBLIC commands)
target_link_libraries(Byteman PUBLIC cli)