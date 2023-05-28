#pragma once

#include <stdint.h>


namespace format {
    
    
    inline __attribute__((always_inline)) int pixelCoord(int width, int x, int y) {
        return (width * y) + x;
    }
    
    inline __attribute__((always_inline)) uint32_t flipColors(uint32_t color) {
        uint32_t red = color & 0xff;
        uint32_t green = (color >> 8) & 0xff;
        uint32_t blue = (color >> 16) & 0xff;
        return blue | (green << 8) | (red << 16);
    }
    
    /**
     * The default X11 color mask seems to be red at the high bits and blue at the low bits (apparently using a 4 byte word?), so on little-endian systems,
     * the colors returned from GL have to be flipped before displaying.
     * Can optionally flip the X axis of the image for X11.
     * 
     */
    inline __attribute__((always_inline)) void GLColorToX11(uint32_t* pixels, int width, int height, bool flipX) {
        #if __BYTE_ORDER == __LITTLE_ENDIAN
            for (int x = 0; x < width; x++) {
                for (int y = 0; y < height; y++) {
                    if (flipX) {
                        if (y >= height/2) break;
                        uint32_t tmp = flipColors(pixels[pixelCoord(width, x, y)]);
                        pixels[pixelCoord(width, x, y)] = flipColors(pixels[pixelCoord(width, x, height - y -1)]);
                        pixels[pixelCoord(width, x, height - y - 1)] = tmp;
                    } else {
                        pixels[pixelCoord(width, x, y)] = flipColors(pixels[pixelCoord(width, x, y)]);
                    }
                    
                }
            }
        #else
            if (flipX) {
                for (int x = 0; x < width; x++) {
                    for (int y = 0; y < height; y++) {
                        uint32_t tmp = pixels[pixelCoord(width, x, y)];
                        pixels[pixelCoord(width, x, y)] = pixels[pixelCoord(width, x, height - y -1)];
                        pixels[pixelCoord(width, x, height - y - 1)] = tmp;
                    }
                }
            }
        #endif
        // Nothing to do on big endian, if there are even Android systems with that.
    }
    
    
    
    
    
}



