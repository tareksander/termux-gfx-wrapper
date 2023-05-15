cmake_minimum_required(VERSION 3.18.4)

project(termux-gfx-wrapper)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(X11_PLATFORM "Provide X11 drawing though libxcb" ON)
option(WAYLAND_PLATFORM "Provide Wayland drawing though libwayland and the hardware buffer exchange protocol if supported" OFF)
set(EGL_DEFAULT_DISPLAY_PLAYFORM "X11" CACHE STRING "The default EGLDisplay platform: NATIVE for pass-through of Android libEGL, WAYLAND or X11.")



# Termux dependency packages: libglvnd, libx11 libxcb
# Termux build dependencies: vulkan-headers, xorgproto, libglvnd-dev, mesa-dev, libx11 libxcb
# without wayland

set(sources-egl
    egl
    dispatch
    android-display
    x11-display
    gles
)
list(TRANSFORM sources-egl PREPEND src/egl/)
list(TRANSFORM sources-egl APPEND .cpp)

set(sources-vulkan
    vulkan
)
list(TRANSFORM sources-vulkan PREPEND src/vulkan/)
list(TRANSFORM sources-vulkan APPEND .cpp)



add_library(vulkan-android SHARED ${sources-vulkan} wayland/hwbuf.c src/common/android.cpp)
target_link_libraries(vulkan-android dl)
if(X11_PLATFORM)
    target_link_libraries(vulkan-android dl xcb X11 X11-xcb)
    target_compile_definitions(vulkan-android PUBLIC X11_PLATFORM)
endif()
if(WAYLAND_PLATFORM)
    target_link_libraries(vulkan-android dl wayland)
    target_compile_definitions(vulkan-android PUBLIC WAYLAND_PLATFORM)
endif()
if(ANDROID EQUAL 1)
target_link_libraries(vulkan-android android)
else()
target_compile_options(vulkan-android PUBLIC -Wall -Wpedantic)
endif()
target_include_directories(vulkan-android PRIVATE wayland src/common)




add_library(egl-android SHARED ${sources-egl} src/common/android.cpp)
target_link_libraries(egl-android dl)
if(X11_PLATFORM)
    target_link_libraries(egl-android dl xcb X11 X11-xcb)
    target_compile_definitions(egl-android PUBLIC X11_PLATFORM)
endif()
if(WAYLAND_PLATFORM)
    target_link_libraries(egl-android dl wayland)
    target_compile_definitions(egl-android PUBLIC WAYLAND_PLATFORM)
endif()
if(ANDROID EQUAL 1)
target_link_libraries(egl-android android)
else()
target_compile_options(egl-android PUBLIC -Wall -Wpedantic)
endif()
target_compile_definitions(egl-android PRIVATE DEFAULT_DISPLAY_PLAYFORM=${EGL_DEFAULT_DISPLAY_PLAYFORM})
target_include_directories(egl-android PRIVATE wayland src/common)

install(TARGETS egl-android LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
install(FILES 10_android_wrapper.json DESTINATION ${CMAKE_INSTALL_PREFIX}/share/glvnd/egl_vendor.d/)


 