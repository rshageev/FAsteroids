cmake_minimum_required(VERSION 3.16)
project(Asteroids LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG master)
FetchContent_MakeAvailable(SFML)

add_executable(Asteroids
    src/main.cpp
    src/System.cpp
    src/Asteroids.cpp
    src/Render.cpp
    src/game/GameLogic.cpp
    src/game/Collisions.cpp
    src/menu/MenuLogic.cpp)
target_include_directories(Asteroids PRIVATE src)
target_link_libraries(Asteroids PRIVATE sfml-graphics)
target_compile_features(Asteroids PRIVATE cxx_std_23)

add_custom_command(
    TARGET Asteroids
    COMMENT "Copy game resources"
    POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/res $<TARGET_FILE_DIR:Asteroids>/res
)

if(WIN32)
    add_custom_command(
        TARGET Asteroids
        COMMENT "Copy OpenAL DLL"
        PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:Asteroids>
        VERBATIM)
endif()

install(TARGETS Asteroids)
