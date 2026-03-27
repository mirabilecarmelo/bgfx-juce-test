cmake_minimum_required(VERSION 3.22)
project(bgfx_juce_test VERSION 1.0.0)

# JUCE path — override with -DJUCE_PATH=/your/path
if(NOT DEFINED JUCE_PATH)
    if(WIN32)
        set(JUCE_PATH "C:/Users/carme/Desktop/installers/JUCE")
    elseif(APPLE)
        set(JUCE_PATH "$ENV{HOME}/JUCE")
    else()
        set(JUCE_PATH "/usr/local/JUCE")
    endif()
endif()

message(STATUS "JUCE_PATH: ${JUCE_PATH}")
add_subdirectory(${JUCE_PATH} ${CMAKE_BINARY_DIR}/juce)

# bgfx
set(BGFX_BUILD_TOOLS    OFF CACHE BOOL "")
set(BGFX_BUILD_EXAMPLES OFF CACHE BOOL "")
set(BGFX_INSTALL        OFF CACHE BOOL "")
set(BGFX_CUSTOM_TARGETS OFF CACHE BOOL "")
add_subdirectory(bgfx.cmake)

# JUCE GUI App
juce_add_gui_app(bgfx_juce_test
    PRODUCT_NAME "bgfx JUCE Test"
    COMPANY_NAME "MUDI"
)

set_target_properties(bgfx_juce_test PROPERTIES CXX_STANDARD 20)

target_sources(bgfx_juce_test PRIVATE
    src/Main.cpp
    src/MainComponent.cpp
    src/BgfxComponent.cpp
)

target_include_directories(bgfx_juce_test PRIVATE src)

target_compile_definitions(bgfx_juce_test PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:bgfx_juce_test,JUCE_PRODUCT_NAME>"
    JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:bgfx_juce_test,JUCE_VERSION>"
)

target_link_libraries(bgfx_juce_test PRIVATE
    juce::juce_core
    juce::juce_gui_basics
    juce::juce_gui_extra
    juce::juce_graphics
    bgfx
    bx
    bimg
    juce::juce_recommended_config_flags
)

# macOS: link system frameworks needed by bgfx Metal backend
if(APPLE)
    target_link_libraries(bgfx_juce_test PRIVATE
        "-framework Metal"
        "-framework MetalKit"
        "-framework QuartzCore"
        "-framework Cocoa"
        "-framework IOKit"
    )
endif()
