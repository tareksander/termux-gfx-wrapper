# termux-gfx-wrapper

Wrapping the native `libEGL.so`, (and `libVulkan.so` in the future), so applications can use hardware acceleration for GLES/Vulkan for X11 (and Wayland in the future).


## TODO

See the GitHub issues.


## Installing

````bash
# You may need to `pkg i x11-repo` if you haven't already
pkg i build-essential libglvnd libx11 libxcb vulkan-headers xorgproto libglvnd-dev mesa-dev libx11 libxcb
git clone https://github.com/tareksander/termux-gfx-wrapper.git
cd termux-gfx-wrapper
cmake -DCMAKE_INSTALL_PREFIX="$PREFIX" .
make install
````

## Using

The wrapper has precedence over the Mesa drivers, so you don't need to turn it on, but you do ned to turn it off, e.g. because you need the full GL support provided by Mesa's llvmpipe.  
Use `export __EGL_VENDOR_LIBRARY_FILENAMES="$PREFIX/share/glvnd/egl_vendor.d/50_mesa.json"` to force the usage of Mesa drivers for EGL.


To verify it's working install the package `mesa-demos` and run `eglinfo`.  
If the vendor string says "Termux EGL wrapper", it's working.
