cmake_minimum_required(VERSION 3.11...3.25)

set(PROJECT_NAME coffee-editor)
set(SHADERS_PROJECT shaders-recompile)
project(${PROJECT_NAME} CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
set(SHADERS_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}/nativefs/shaders")

file(COPY_FILE "${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui-cmake.patch" "${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui/CMakeLists.txt")
file(COPY_FILE "${CMAKE_CURRENT_SOURCE_DIR}/libs/imguifiledialog/imgui_file_dialog.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui/imgui_file_dialog.cpp")
file(COPY_FILE "${CMAKE_CURRENT_SOURCE_DIR}/libs/imguifiledialog/imgui_file_dialog.h" "${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui/imgui_file_dialog.h")
file(COPY_FILE "${CMAKE_CURRENT_SOURCE_DIR}/libs/imguifiledialog/imgui_file_dialog_config.h" "${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui/imgui_file_dialog_config.h")
file(COPY_FILE "${CMAKE_CURRENT_SOURCE_DIR}/libs/imguifiledialog/imgui_file_dialog_def.h" "${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui/imgui_file_dialog_def.h")
execute_process(COMMAND cmd /C "cd ${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui && git apply ../imgui-config.patch" OUTPUT_QUIET ERROR_QUIET)

if(MSVC)
    add_compile_options(/GR-)
    add_compile_options(/EHa)
endif()

add_subdirectory(engine)
add_subdirectory(libs/imgui)

find_package(Vulkan)

file(GLOB_RECURSE SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/nativefs/shaders/*.vert"
    "${CMAKE_CURRENT_SOURCE_DIR}/nativefs/shaders/*.frag")

add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/entt"
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui"
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/imguifiledialog"
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/nlohmann")
target_link_libraries(${PROJECT_NAME} PRIVATE coffee-engine imgui)

file(GLOB_RECURSE SHADERS_SOURCES "${SHADERS_FOLDER}/*.vert" "${SHADERS_FOLDER}/*.frag")
foreach(SOURCE IN ITEMS ${SHADERS_SOURCES})
    add_custom_command(OUTPUT "${SOURCE}.spv" COMMAND ${SHADERS_FOLDER}/compile_shader.bat "${SOURCE}" DEPENDS "${SOURCE}" VERBATIM)
    list(APPEND DEPENDENCIES_SHADERS "${SOURCE}.spv")
endforeach()
add_custom_target(${SHADERS_PROJECT} DEPENDS ${DEPENDENCIES_SHADERS})
add_dependencies(${PROJECT_NAME} ${SHADERS_PROJECT})
list(APPEND SOURCES ${SHADERS_SOURCES})

if(MSVC)
    set_target_properties(imgui glm PROPERTIES
        VS_GLOBAL_VcpkgEnabled FALSE
        FOLDER "coffee-editor-libs"
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
    )
    set_target_properties(${PROJECT_NAME} PROPERTIES
        VS_GLOBAL_VcpkgEnabled FALSE
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
    )
    set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" PROPERTY VS_STARTUP_PROJECT "${PROJECT_NAME}")
    
    foreach(SOURCE IN ITEMS ${SOURCES})
    	get_filename_component(SOURCE_PATH "${SOURCE}" PATH)
    	string(REPLACE "${CMAKE_SOURCE_DIR}" "" GROUP_PATH "${SOURCE_PATH}")
    	string(REPLACE "/" "\\" GROUP_PATH "${GROUP_PATH}")
    	source_group("${GROUP_PATH}" FILES "${SOURCE}")
    endforeach()
endif()