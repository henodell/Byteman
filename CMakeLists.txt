cmake_minimum_required(VERSION 3.10...4.1)
project(Byteman VERSION 0.1.0 LANGUAGES C)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(PROJECT_INCLUDE_DIR include)
set(LOCKED_COMMANDS_DIR "src/commands/locked")
set(OPEN_COMMANDS_DIR "src/commands/open")
set(CLI_DIR "src/cli")
set(UTILS_DIR "src/utils")
set(DEPS_DIR ${PROJECT_SOURCE_DIR}/deps)

# Packages
find_package(OpenSSL REQUIRED)
find_package(Doxygen)

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

# cjson
add_library(cJSON STATIC ${DEPS_DIR}/cjson/cJSON.c)
target_include_directories(cJSON PUBLIC ${DEPS_DIR}/cjson/)

# Open Commands
add_library(open_commands STATIC ${OPEN_COMMANDS_DIR}/Get.c)
target_include_directories(open_commands PUBLIC include) 

# Locked Commands
add_library(locked_commands STATIC ${LOCKED_COMMANDS_DIR}/Login.c ${LOCKED_COMMANDS_DIR}/Signup.c)
target_include_directories(locked_commands PUBLIC include)
target_link_libraries(locked_commands PRIVATE OpenSSL::Crypto cJSON)

# ClI parsing
add_library(cli STATIC ${CLI_DIR}/CliParse.c ${CLI_DIR}/CommandDispatch.c)
target_include_directories(cli PUBLIC include)
target_link_libraries(cli PUBLIC locked_commands open_commands)

# main exe
add_executable(Byteman src/Main.c ${UTILS_DIR}/Crypto.c ${UTILS_DIR}/Utils.c)
target_link_libraries(Byteman PUBLIC locked_commands open_commands cli)