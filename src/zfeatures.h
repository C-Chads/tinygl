#ifndef _tgl_features_h_
#define _tgl_features_h_

/* It is possible to enable/disable (compile time) features in this
   header file. */

#define TGL_FEATURE_ARRAYS         1
#define TGL_FEATURE_DISPLAYLISTS   1
#define TGL_FEATURE_POLYGON_OFFSET 1

#define TGL_FEATURE_POLYGON_STIPPLE 1
//A stipple pattern is 128 bytes in size.
#define TGL_POLYGON_STIPPLE_BYTES 128
//A stipple pattern is 2^5 (32) bits wide.
#define TGL_POLYGON_STIPPLE_POW2_WIDTH 5
//The stipple pattern mask (the last bits of the screen coordinates used for indexing)
//The default pattern is 32 bits wide and 32 bits tall, or 4 bytes per row and 32 tall, 4 * 32 128 bytes.
#define TGL_POLYGON_STIPPLE_MASK_X 31
#define TGL_POLYGON_STIPPLE_MASK_Y 31

//These are features useful for integrating TinyGL with other renderers.
#define TGL_FEATURE_NO_COPY_COLOR 0
#define TGL_FEATURE_FORCE_CLEAR_NO_COPY_COLOR 0
#define TGL_NO_COPY_COLOR 0xff00ff
//NOTE: fc02fc is what you'll get if you set glColor3f to 1,0,1. 
//^ solid debug pink.
#define TGL_COLOR_MASK 0xffffffff
//^ mask to check for copybuffer. This is configured for the default mode.

/*
 * Matrix of internal and external pixel formats supported. 'Y' means
 * supported.
 * 
 *           External  8    16    24    32
 * Internal 
 *  15                 .     .     .     .
 *  16                 Y     Y     Y     Y
 *  24                 .     Y     Y     .
 *  32                 .     Y     .     Y
 * 
 *
 * 15 bpp does not work yet (although it is easy to add it - ask me if
 * you need it).
 * 
 * Internal pixel format: see TGL_FEATURE_RENDER_BITS
 * External pixel format: see TGL_FEATURE_xxx_BITS 
 */

/* enable various convertion code from internal pixel format (usually
   16 bits per pixel) to any external format */
//CURRENT MAINTAINER'S NOTE: Only FEATURE_32_BITS is being maintained.
#define TGL_FEATURE_16_BITS        0
#define TGL_FEATURE_8_BITS         0
#define TGL_FEATURE_24_BITS        0
#define TGL_FEATURE_32_BITS        1

//MAINTAINER'S NOTE: Only TGL_FEATURE_RENDER_BITS 32 is maintained.
//All others are experimental.
//24 bit is broken.
//#define TGL_FEATURE_RENDER_BITS    15
//#define TGL_FEATURE_RENDER_BITS    16
//#define TGL_FEATURE_RENDER_BITS    24 //BROKEN!
#define TGL_FEATURE_RENDER_BITS    32

#endif /* _tgl_features_h_ */
