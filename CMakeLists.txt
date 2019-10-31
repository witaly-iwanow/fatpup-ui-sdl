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
find_package(SDL2_ttf REQUIRED >=2.0.0)

add_subdirectory(fatpup)

add_executable(fatpup-ui board.cpp board.h colors.h fatpup-ui.cpp movepanel.cpp movepanel.h)
target_include_directories(fatpup-ui PRIVATE ${FATPUP_INCLUDE_DIR} ${SDL2_INCLUDE_DIR} ${SDL2_IMAGE_INCLUDE_DIR} ${SDL2_TTF_INCLUDE_DIR})

target_link_libraries(fatpup-ui ${FATPUP_LIBRARIES} SDL2::Main SDL2::Image SDL2::TTF)
if (UNIX AND NOT APPLE)
    target_link_libraries(fatpup-ui pthread)
endif()

add_custom_command(TARGET fatpup-ui POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${CMAKE_SOURCE_DIR}/resources $<TARGET_FILE_DIR:fatpup-ui>/resources)

if (WIN32)

# VS adds Debug/Release to the target path, but not to the debugger working directory makes. At the same
# time set_target_properties/VS_DEBUGGER_WORKING_DIRECTORY doesn't help as we need generator expression
# $<TARGET_FILE_DIR:fatpup-ui> which is not available for set_target_properties.
# Just copy resources twice for now and enjoy VS running/debugging out-of-the-box
add_custom_command(TARGET fatpup-ui POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${CMAKE_SOURCE_DIR}/resources $<TARGET_FILE_DIR:fatpup-ui>/../resources)

get_filename_component(SDL2_DLL_FOLDER ${SDL2_LIBRARY} DIRECTORY)
file (GLOB SDL2_DLLS ${SDL2_DLL_FOLDER}/*.dll)
get_filename_component(SDL2_IMAGE_DLL_FOLDER ${SDL2_IMAGE_LIBRARY} DIRECTORY)
file (GLOB SDL2_IMAGE_DLLS ${SDL2_IMAGE_DLL_FOLDER}/*.dll)
get_filename_component(SDL2_TTF_DLL_FOLDER ${SDL2_TTF_LIBRARY} DIRECTORY)
file (GLOB SDL2_TTF_DLLS ${SDL2_TTF_DLL_FOLDER}/*.dll)
add_custom_command(TARGET fatpup-ui POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy
                   ${SDL2_DLLS} ${SDL2_IMAGE_DLLS} ${SDL2_TTF_DLLS} $<TARGET_FILE_DIR:fatpup-ui>)
endif()
