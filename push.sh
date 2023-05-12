#!/bin/bash

if [ ! -f termux-gfx-wrapper.tar.gz ] || [ "$(tar df termux-gfx-wrapper.tar.gz)" != "" ]; then
    tar caf termux-gfx-wrapper.tar.gz CMakeLists.txt src wayland 10_android_wrapper.json
fi

"$ANDROID_HOME"/platform-tools/adb push --sync termux-gfx-wrapper.tar.gz /storage/emulated/0/Download/termux-gfx-wrapper.tar.gz

