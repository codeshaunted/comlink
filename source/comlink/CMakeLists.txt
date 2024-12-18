set(COMLINK_SOURCE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/dll_main.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/util.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/d3d9.def"
    "${CMAKE_SOURCE_DIR}/third_party/imgui/imgui.cpp"
    "${CMAKE_SOURCE_DIR}/third_party/imgui/imgui_draw.cpp"
    "${CMAKE_SOURCE_DIR}/third_party/imgui/imgui_tables.cpp"
    "${CMAKE_SOURCE_DIR}/third_party/imgui/imgui_widgets.cpp"
    "${CMAKE_SOURCE_DIR}/third_party/imgui/backends/imgui_impl_dx9.cpp"
    "${CMAKE_SOURCE_DIR}/third_party/imgui/backends/imgui_impl_win32.cpp")

set(COMLINK_COMPILE_DEFINITIONS
    WIN32_LEAN_AND_MEAN
    WINDOWS_IGNORE_PACKING_MISMATCH)

set(COMLINK_INCLUDE_DIRECTORIES
    "${CMAKE_SOURCE_DIR}/include/comlink"
    "${CMAKE_SOURCE_DIR}/third_party/imgui"
    "${CMAKE_SOURCE_DIR}/third_party/minhook/include"
    ${PROJECT_BINARY_DIR})

set(COMLINK_LINK_LIBRARIES
    minhook)

add_library(d3d9 SHARED ${COMLINK_SOURCE_FILES})

target_compile_definitions(d3d9 PUBLIC ${COMLINK_COMPILE_DEFINITIONS})

target_include_directories(d3d9 PUBLIC ${COMLINK_INCLUDE_DIRECTORIES})

target_link_libraries(d3d9 PUBLIC ${COMLINK_LINK_LIBRARIES})

# todo: add way to configure this
install(TARGETS d3d9 DESTINATION "C:/Program Files (x86)/Steam/steamapps/common/Lego Star Wars Saga")