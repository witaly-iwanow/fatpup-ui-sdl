cmake_minimum_required(VERSION 3.7)
project(fatpup-ui)

if (NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE Release)
    ADD_DEFINITIONS(-DNDEBUG)
endif()

set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_STANDARD 11)

find_package(SDL2 REQUIRED)

add_subdirectory(fatpup)

add_executable(fatpup-ui fatpup-ui.cpp umax4_8.c)
target_include_directories(fatpup-ui PRIVATE ${FATPUP_INCLUDE_DIR} ${SDL2_INCLUDE_DIRS})
string(STRIP ${SDL2_LIBRARIES} SDL2_LIBRARIES)
target_link_libraries(fatpup-ui ${FATPUP_LIBRARIES} ${SDL2_LIBRARIES})
add_custom_command(TARGET fatpup-ui POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${CMAKE_SOURCE_DIR}/resources $<TARGET_FILE_DIR:fatpup-ui>/resources)