# For modules
cmake_minimum_required(VERSION 3.28.3)

project(mandelbrot LANGUAGES CXX VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_COLOR_DIAGNOSTICS ON)

option(ENABLE_SEQUENTIAL "Enable sequential CPU implementation" ON)
option(ENABLE_OPENMP "Enable OpenMP acceleration" OFF)
option(ENABLE_HIP "Enable HIP GPU acceleration" OFF)
option(ENABLE_SANITIZERS "Enable Address and Memory Sanitizers" OFF)

if(ENABLE_OPENMP AND ENABLE_HIP)
  message(FATAL_ERROR "HIP and OPENMP can't be enable together")
endif()

if(${ENABLE_HIP})
  set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
  find_package(HIP QUIET)
  if(NOT DEFINED ROCM_PATH)
    message(WARNING "Adding /opt/rocm to CMAKE_PREFIX_PATH")
    list(APPEND CMAKE_PREFIX_PATH "/opt/rocm")
  else()
    message(STATUS "ROCM_PATH: ${ROCM_PATH}")
    list(APPEND CMAKE_PREFIX_PATH ${ROCM_PATH})
    set(CMAKE_BUILD_RPATH_USE_ORIGIN TRUE)
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
    link_directories("${ROCM_PATH}/lib")
    include_directories("${ROCM_PATH}/include")
  endif()
  find_package(HIP QUIET)
  if(${HIP_FOUND})
    message(STATUS "Found HIP package")
    set(CMAKE_CXX_COMPILER ${HIPCC})
  endif()
  find_program(HIPCC hipcc)
  if(${HIPCC})
    message(STATUS "Found hipcc: ${HIPCC}")
    set(CMAKE_CXX_COMPILER ${HIPCC})
  endif()
endif()

if(${ENABLE_OPENMP})
  find_package(OpenMP QUIET)
  if(${OPENMP_FOUND})
    message(STATUS "Found OPENMP")
  else()
    message(WARNING "OPENMP not found switching to Serial")
    set(${ENABLE_OPENMP} OFF)
    set(${ENABLE_SERIAL} ON)
  endif()
endif()

find_package(glfw3 REQUIRED)

# sanitizers
if(ENABLE_SANITIZERS)
  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang" AND NOT ENABLE_HIP)
    message(STATUS "Enabling Address and Memory Sanitizers")
    # -fsanitize=undefined -fsanitize=adress -fsanitize=memory
    add_compile_options(-fsanitize=memory -fno-omit-frame-pointer)
    add_link_options(-fsanitize=memory)
  endif()
  if(ENABLE_HIP)
    add_compile_options(-fgpu-sanitize -Rpass-analysis=loop-vectorize
                        -Rpass-analysis=kernel-resource-usag -Rpass=loop-vectorize)
  endif()
endif()

# FREEIMAGE stuff
find_library(FREEIMAGE_LIBRARY freeimage)
find_path(FREEIMAGE_INCLUDE_DIR FreeImage.h)
message(STATUS "Found FreeImage library: ${FREEIMAGE_LIBRARY}")
message(STATUS "Found FreeImage headers: ${FREEIMAGE_INCLUDE_DIR}")

set(MANDELBROT_SRC
  src/core/global_config.cpp
  src/core/window_utils.cpp
  src/gl/gl_event.cpp
  src/gl/gl_render.cpp
  src/render/save_image.cpp
  src/render/color_scheme.cpp
  src/glfw/glfw_utils.cpp
  src/main.cpp
  external/glad/glad.cpp
)

if(${ENABLE_HIP})
  list(APPEND MANDELBROT_SRC src/fractal/mandel_hip.cpp)
elseif(${ENABLE_OPENMP})
 list(APPEND MANDELBROT_SRC src/fractal/mandel_openmp.cpp)
else()
  list(APPEND MANDELBROT_SRC src/fractal/mandel_sequential.cpp)
endif()

add_compile_options(-march=native -Wall -Wextra)

add_executable(mandelbrot ${MANDELBROT_SRC})
target_link_libraries(mandelbrot PRIVATE ${FREEIMAGE_LIBRARY} glfw)
target_include_directories(mandelbrot PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/external/glad ${CMAKE_CURRENT_SOURCE_DIR}/external/khr)

if(${ENABLE_HIP})
  target_compile_definitions(mandelbrot PUBLIC USE_HIP)
  target_include_directories(mandelbrot PUBLIC ${HIP_INCLUDE_DIRS})
  target_link_libraries(mandelbrot PRIVATE ${HIP_LIBRARIES})
else()
  target_sources(mandelbrot
    PUBLIC FILE_SET CXX_MODULES FILES
    src/core/global_config.cppm
  )
endif()

if(${ENABLE_SEQUENTIAL})
  target_compile_definitions(mandelbrot PUBLIC USE_SEQUENTIAL)
endif()

if(${ENABLE_OPENMP})
  target_compile_definitions(mandelbrot PUBLIC USE_OPENMP)
  target_link_libraries(mandelbrot PRIVATE OpenMP::OpenMP_CXX)
endif()

message(STATUS "CMAKE Status:")
message(STATUS "CMAKE_CXX_COMPILER: ${CMAKE_CXX_COMPILER}")
message(STATUS "CMAKE_CXX_COMPILER_ID: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "CMAKE_CXX_COMPILER_VERSION: ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
message(STATUS "CMAKE_CXX_STANDARD: ${CMAKE_CXX_STANDARD}")
message(STATUS "CMAKE_BUILD_RPATH: ${CMAKE_BUILD_RPATH}")
message(STATUS "CMAKE_INSTALL_RPATH: ${CMAKE_INSTALL_RPATH}")
message(STATUS "CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")
message(STATUS "CMAKE_MODULE_PATH: ${CMAKE_MODULE_PATH}")
message(STATUS "CMAKE_BINARY_DIR: ${CMAKE_BINARY_DIR}")
message(STATUS "CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")
execute_process(COMMAND git log -n 1 --format="%h %an %cd %s"
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/${BRANCH}
                OUTPUT_VARIABLE GIT_LOG
                OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND git branch --show-current -v
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/${BRANCH}
                OUTPUT_VARIABLE GIT_BRANCH
                OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND git status -s
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/${BRANCH}
                OUTPUT_VARIABLE MODIFIED_FILES
                OUTPUT_STRIP_TRAILING_WHITESPACE)
message(STATUS "GIT Status:")
message(STATUS "Git log:\n${GIT_LOG}")
message(STATUS "Current branch:\n${GIT_BRANCH}")
message(STATUS "Modified files:\n${MODIFIED_FILES}")
