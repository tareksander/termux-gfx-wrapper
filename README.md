# termux-gfx-wrapper



## TODO


[ ] Android EGL and GLES passthrough with libglvnd
[ ] X11 shared memory based rendering with glReadPixels to the shared memory
[ ] putImage fallback if shared memory isn't available
[ ] getting the DMABUF fd from AHardwareBuffer and using that for X11 shared memory
 - can fail if the HardwareBuffer flattened representation changes
 - Test during install-time and disable if needed
 - no patches to X servers needed
[ ] X11 extension for hardware buffers?
 - patch for xwayland and tigervnc packages
 - should work always, because it's in the ndk
[ ] GL4ES integration for GL support
[ ] Wayland shared memory based rendering with glReadPixels to the shared memory
[ ] Wayland extension for hardware buffers
[ ] Vulkan Android passthrough
[ ] X11 shared memory rendering for Vulkan
[ ] X11 hardware buffer rendering for Vulkan
[ ] Wayland shared memory rendering for Vulkan
[ ] Wayland hardware buffer rendering for Vulkan















