cmake_minimum_required(VERSION 3.15)
project(RenderLib LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# collect all sources in src/
file(GLOB RENDERLIB_SOURCES CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/src/*.cpp")
add_library(RenderLib ${RENDERLIB_SOURCES})

target_include_directories(RenderLib PUBLIC include)

# Expect user to provide glad and glm via find_package or include paths
find_package(glm QUIET)
if(glm_FOUND)
    target_include_directories(RenderLib PUBLIC ${GLM_INCLUDE_DIRS})
endif()

message(STATUS "Note: glad and glm must be provided by the consumer (include + library).")

install(TARGETS RenderLib DESTINATION lib)
install(DIRECTORY include/ DESTINATION include)
