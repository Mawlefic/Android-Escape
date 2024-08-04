cmake_minimum_required(VERSION 3.11) # FetchContent is available in 3.11+
project("MVaniaJam 2024")

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Dependencies

set(RAYLIB_VERSION 5.0)
find_package(raylib ${RAYLIB_VERSION} QUIET) # QUIET or REQUIRED
if (NOT raylib_FOUND) # If there's none, fetch and build raylib
  include(FetchContent)
  FetchContent_Declare(
    raylib
    DOWNLOAD_EXTRACT_TIMESTAMP OFF
    URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
  )
  FetchContent_GetProperties(raylib)
  if (NOT raylib_POPULATED) # Have we downloaded raylib yet?
    set(FETCHCONTENT_QUIET NO)
    FetchContent_Populate(raylib)
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
    add_subdirectory(${raylib_SOURCE_DIR} ${raylib_BINARY_DIR})
  endif()
endif()

# Our Project
add_executable (MvaniaJam_2024 "main.cpp" "game_state/states.h" "game_state/states.cpp" "entity/collision_object.h" "entity/collision_object.cpp" "entity/tile.h" "entity/tile.cpp" "entity/player.h" "entity/player.cpp" "helper/utility.h" "helper/utility.cpp" "entity/projectile.h" "entity/projectile.cpp" "game_state/renderer.cpp" "game_state/renderer.h" "entity/entity.h" "game_state/world.h" "game_state/world.cpp" "entity/enemy.h" "entity/enemy.cpp"  "entity/collectible.cpp" "entity/collectible.h" "entity/textbox.h" "entity/textbox.cpp")
#set(raylib_VERBOSE 1)
target_link_libraries(MvaniaJam_2024 raylib)
file(COPY map_data DESTINATION ${CMAKE_BINARY_DIR})
file(COPY shaders DESTINATION ${CMAKE_BINARY_DIR})
file(COPY sprites DESTINATION ${CMAKE_BINARY_DIR})
file(COPY audio DESTINATION ${CMAKE_BINARY_DIR})

# Web Configurations
if (${PLATFORM} STREQUAL "Web")
    # Tell Emscripten to build an example.html file.
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html")
endif()

# Checks if OSX and links appropriate frameworks (Only required on MacOS)
if (APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif()