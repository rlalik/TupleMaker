cmake_minimum_required(VERSION 3.14)

project(TupleMakerExamples CXX)

include(../cmake/project-is-top-level.cmake)
include(../cmake/folders.cmake)

# ---- Extra dependencies ----

include(FetchContent)

FetchContent_Declare(
  hades-tools
  GIT_REPOSITORY https://git.gsi.de/r.lalik/hades-tools
  GIT_TAG master
)
FetchContent_MakeAvailable(hades-tools)
list(APPEND CMAKE_MODULE_PATH ${hades-tools_SOURCE_DIR}/cmake)

find_package(HYDRA)

# ---- Extra dependencies ----

if(PROJECT_IS_TOP_LEVEL)
  find_package(TupleMaker REQUIRED)
endif()

add_custom_target(run-examples)

function(add_example NAME)
  add_executable("${NAME}" "${NAME}.cpp")
  target_link_libraries("${NAME}" PRIVATE TupleMaker::TupleMaker ROOT::RIO ROOT::Tree)
  target_compile_features("${NAME}" PRIVATE cxx_std_17)
  add_custom_target("run_${NAME}" COMMAND "${NAME}" VERBATIM)
  add_dependencies("run_${NAME}" "${NAME}")
  add_dependencies(run-examples "run_${NAME}")
endfunction()

add_example(basic_example)

if (HYDRA_FOUND)
  add_example(hades_example)
  target_link_libraries(hades_example PRIVATE HYDRA::all)
endif()

add_folders(Example)
