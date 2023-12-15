cmake_minimum_required(VERSION 3.14)

include(cmake/prelude.cmake)

project(
    scenegraph
    VERSION 0.1.0
    LANGUAGES CXX
)

include(cmake/project-is-top-level.cmake)
include(cmake/variables.cmake)

# ---- Declare library ----

add_library(scenegraph_lib OBJECT
    source/lib.cpp
)

target_include_directories(
    scenegraph_lib ${warning_guard}
    PUBLIC
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
)

target_compile_features(scenegraph_lib PUBLIC cxx_std_20)

find_package(assimp REQUIRED)
target_link_libraries(scenegraph_lib PRIVATE assimp::assimp)
find_package(fmt REQUIRED)
target_link_libraries(scenegraph_lib PRIVATE fmt::fmt)
find_package(glm REQUIRED)
target_link_libraries(scenegraph_lib PRIVATE glm::glm)

# ---- Declare executable ----

add_executable(scenegraph_exe source/main.cpp)
add_executable(scenegraph::exe ALIAS scenegraph_exe)

set_property(TARGET scenegraph_exe PROPERTY OUTPUT_NAME scenegraph)

target_compile_features(scenegraph_exe PRIVATE cxx_std_20)

target_link_libraries(scenegraph_exe PRIVATE scenegraph_lib)

# ---- Install rules ----

if(NOT CMAKE_SKIP_INSTALL_RULES)
  include(cmake/install-rules.cmake)
endif()

# ---- Developer mode ----

if(NOT scenegraph_DEVELOPER_MODE)
  return()
elseif(NOT PROJECT_IS_TOP_LEVEL)
  message(
      AUTHOR_WARNING
      "Developer mode is intended for developers of scenegraph"
  )
endif()

include(cmake/dev-mode.cmake)
