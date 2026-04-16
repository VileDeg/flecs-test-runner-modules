set(LIB_NAME "Modules")
set(SRC_DIR "./")

include(FetchContent)

# Fetch the Flecs repository
FetchContent_Declare(
    flecs
    GIT_REPOSITORY https://github.com/SanderMertens/flecs.git
)
FetchContent_MakeAvailable(flecs)

# Explicit source file list (no GLOB_RECURSE)
set(SRC_FILES
    movement.cpp
    structural.cpp
    time.cpp
    modifiers.cpp
    include/modules/common.h
    include/modules/movement.h
    include/modules/structural.h
    include/modules/modifiers.h
    include/modules/time.h
)

# Create a static library
add_library(
  ${LIB_NAME} STATIC 
  ${SRC_FILES}
)

# Add src directory to include path so headers can be found
target_include_directories(
  ${LIB_NAME} 
  PRIVATE 
    ${SRC_DIR}
  PUBLIC
    ${SRC_DIR}/include
)

target_link_libraries(
  ${LIB_NAME}
  PRIVATE
    flecs::flecs_static
)
