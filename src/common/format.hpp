#pragma once

#include <stdint.h>


namespace format {
    
    /**
     * The default X11 color mask seems to be red at the high bits and blue at the low bits (apparently using a 4 byte word?), so on little-endian systems,
     * the colors returned from GL have to be flipped before displaying.
     * Can optionally flip the X axis of the image for X11.
     * 
     */
    void GLColorToX11(uint32_t* pixels, int width, int height, bool flipX);
    
    
    
    
    
}



