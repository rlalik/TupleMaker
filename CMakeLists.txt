cmake_minimum_required(VERSION 3.14)

include(cmake/prelude.cmake)

project(
    TupleMaker
    VERSION 0.1.0
    DESCRIPTION "Short description"
    HOMEPAGE_URL "https://example.com/"
    LANGUAGES NONE
)

include(cmake/project-is-top-level.cmake)
include(cmake/variables.cmake)

# ---- Dependencies ----

# find ROOT
find_package(ROOT QUIET REQUIRED COMPONENTS Core Physics Tree)

# ---- Declare library ----

add_library(TupleMaker_TupleMaker INTERFACE)
add_library(TupleMaker::TupleMaker ALIAS TupleMaker_TupleMaker)

target_link_libraries(TupleMaker_TupleMaker
    INTERFACE ROOT::Core ROOT::Physics ROOT::Tree
    # PRIVATE ${FMT_TARGET}
)

set_property(
    TARGET TupleMaker_TupleMaker PROPERTY
    EXPORT_NAME TupleMaker
)

target_include_directories(
    TupleMaker_TupleMaker ${warning_guard}
    INTERFACE
    "\$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
)

# target_compile_features(TupleMaker_TupleMaker INTERFACE cxx_std_17)

# ---- Install rules ----

if(NOT CMAKE_SKIP_INSTALL_RULES)
  include(cmake/install-rules.cmake)
endif()

# ---- Examples ----

if(PROJECT_IS_TOP_LEVEL)
  option(BUILD_EXAMPLES "Build examples tree." "${TupleMaker_DEVELOPER_MODE}")
  if(BUILD_EXAMPLES)
    add_subdirectory(example)
  endif()
endif()

# ---- Developer mode ----

if(NOT TupleMaker_DEVELOPER_MODE)
  return()
elseif(NOT PROJECT_IS_TOP_LEVEL)
  message(
      AUTHOR_WARNING
      "Developer mode is intended for developers of TupleMaker"
  )
endif()

include(cmake/dev-mode.cmake)
