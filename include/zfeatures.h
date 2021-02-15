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
//The default pattern is 32 bits wide and 32 bits tall, or 4 bytes per row and 32 tall, 4 * 32 = 128 bytes.
#define TGL_POLYGON_STIPPLE_MASK_X 31
#define TGL_POLYGON_STIPPLE_MASK_Y 31

//These are features useful for integrating TinyGL with other renderers.
#define TGL_FEATURE_NO_COPY_COLOR 0
#define TGL_FEATURE_NO_DRAW_COLOR 0
#define TGL_FEATURE_FORCE_CLEAR_NO_COPY_COLOR 0
#define TGL_NO_COPY_COLOR 0xff00ff
#define TGL_NO_DRAW_COLOR 0xff00ff
//^ solid debug pink.
#define TGL_COLOR_MASK 0x00ffffff
//^ mask to check for while drawing/copying.

/*
 * Matrix of internal and external pixel formats supported. 'Y' means
 * supported.
 * 
 *           External  8    16    24    32
 * Internal 
 *  15                 .     .     .     .
 *  16                 .     Y     .     .
 *  24                 .     .     .     .  (Stupid and pointless)
 *  32                 .     .     .     Y
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
#define TGL_FEATURE_8_BITS         0
#define TGL_FEATURE_24_BITS        0
//These are the only maintained modes.
#define TGL_FEATURE_16_BITS        0
#define TGL_FEATURE_32_BITS        1

//MAINTAINER'S NOTE: Only TGL_FEATURE_RENDER_BITS 32 is maintained.
//TODO: Include support for 16 bit.
//All others are experimental.
//24 bit is broken.
//#define TGL_FEATURE_RENDER_BITS    15
//#define TGL_FEATURE_RENDER_BITS    16
//#define TGL_FEATURE_RENDER_BITS    24 //BROKEN!
#if TGL_FEATURE_32_BITS == 1
#define TGL_FEATURE_RENDER_BITS    32

#elif TGL_FEATURE_16_BITS == 1
#define TGL_FEATURE_RENDER_BITS    16

#else
#error Unsupported TGL_FEATURE_RENDER_BITS

#endif


#endif /* _tgl_features_h_ */
