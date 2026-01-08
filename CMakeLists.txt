cmake_minimum_required(VERSION 3.10...4.1)
project(Byteman VERSION 0.1.0 LANGUAGES C)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(PROJECT_INCLUDE_DIR include)
set(LOCKED_COMMANDS_DIR "src/commands/locked")
set(OPEN_COMMANDS_DIR "src/commands/open")
set(CLI_DIR "src/cli")
set(UTILS_DIR "src/utils")

# Packages
find_package(OpenSSL REQUIRED)
find_package(Doxygen REQUIRED)

# Doxygen
if (DOXYGEN_FOUND)
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs_doxygen/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.out)

    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    message("Doxygen started")

    add_custom_target( docs
    COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating docs"
    VERBATIM
    )
else (DOXYGEN_FOUND)
    message("Doxygen needs to be installed")
endif (DOXYGEN_FOUND)

# Open Commands
add_library(open_commands STATIC ${OPEN_COMMANDS_DIR}/get.c)
target_include_directories(open_commands PUBLIC include)
target_link_libraries(open_commands PRIVATE OpenSSL::Crypto)

# Locked Commands
add_library(locked_commands STATIC ${LOCKED_COMMANDS_DIR}/login.c ${LOCKED_COMMANDS_DIR}/signup.c)
target_include_directories(locked_commands PUBLIC include)
target_link_libraries(locked_commands PRIVATE OpenSSL::Crypto)

# ClI parsing
add_library(cli STATIC ${CLI_DIR}/cli_parse.c ${CLI_DIR}/command_dispatch.c)
target_include_directories(cli PUBLIC include)
target_link_libraries(cli PUBLIC locked_commands)
target_link_libraries(cli PUBLIC open_commands)

# main exe
add_executable(Byteman src/main.c ${UTILS_DIR}/crypto.c ${UTILS_DIR}/utils.c)
target_link_libraries(Byteman PUBLIC locked_commands)
target_link_libraries(Byteman PUBLIC open_commands)
target_link_libraries(Byteman PUBLIC cli)