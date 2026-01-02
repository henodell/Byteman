cmake_minimum_required(VERSION 3.15...4.1)
project(Byteman VERSION 0.1.0 LANGUAGES C)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(COMMANDS_DIR "src/commands")

# Commands lib
add_library(commands STATIC ${COMMANDS_DIR}/get.c)
target_include_directories(commands PUBLIC include)

# main exe
add_executable(Byteman src/main.c)
target_link_libraries(Byteman PUBLIC commands)
