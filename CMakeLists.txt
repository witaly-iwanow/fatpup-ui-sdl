cmake_minimum_required(VERSION 3.7)
project(fatpup-ui)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
    add_definitions(-DNDEBUG)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")

set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_STANDARD 11)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED >=2.0.0)

add_subdirectory(fatpup)

add_executable(fatpup-ui board.cpp fatpup-ui.cpp umax4_8.c)
target_include_directories(fatpup-ui PRIVATE ${FATPUP_INCLUDE_DIR} ${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIR})
string(STRIP ${SDL2_LIBRARIES} SDL2_LIBRARIES)

target_link_libraries(fatpup-ui ${FATPUP_LIBRARIES} ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARY})
if (UNIX AND NOT APPLE)
    target_link_libraries(fatpup-ui pthread)
endif()

add_custom_command(TARGET fatpup-ui POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${CMAKE_SOURCE_DIR}/resources $<TARGET_FILE_DIR:fatpup-ui>/resources)
