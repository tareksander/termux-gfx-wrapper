#include "format.hpp"

#include <endian.h>



static int pixelCoord(int width, int x, int y) {
    return (width * y) + x;
}

static uint32_t flipColors(uint32_t color) {
    uint32_t red = color & 0xff;
    uint32_t green = (color >> 8) & 0xff;
    uint32_t blue = (color >> 16) & 0xff;
    return blue | (green << 8) | (red << 16);
}

void format::GLColorToX11(uint32_t* pixels, int width, int height, bool flipX) {
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