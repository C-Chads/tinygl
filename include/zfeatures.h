#ifndef _tgl_features_h_
#define _tgl_features_h_

/* It is possible to enable/disable (compile time) features in this
   header file. */

//Enables setting the error flags when there's an error, so you can check it with glGetError
//Disabling this has slight performance gains.
#define TGL_FEATURE_ERROR_CHECK 1
//Strict out-of-memory checking. All OpenGL function calls are invalidated (ALL OF THEM) if a GL_OUT_OF_MEMORY error occurs.
//This slows down the renderer so we don't usually do it, but
//it's part of the GL spec.
#define TGL_FEATURE_STRICT_OOM_CHECKS 0
//Clientside Arrays
#define TGL_FEATURE_ARRAYS         1
//This library has a super cool opcode system based on function pointers, it's pretty fast too.
#define TGL_FEATURE_DISPLAYLISTS   1
//Lighting is expensive on texture-mapped polygons.
#define TGL_FEATURE_LIT_TEXTURES   1
//NOTE: Polygon Offset does nothing at the moment.
#define TGL_FEATURE_POLYGON_OFFSET 0
//Enable the patternized "discard"-ing of pixels.
#define TGL_FEATURE_POLYGON_STIPPLE 0
//Enable GL_BLEND functionality
#define TGL_FEATURE_BLEND 			1

//A stipple pattern is 128 bytes in size.
#define TGL_POLYGON_STIPPLE_BYTES 128
//A stipple pattern is 2^5 (32) bits wide.
#define TGL_POLYGON_STIPPLE_POW2_WIDTH 5
//The stipple pattern mask (the last bits of the screen coordinates used for indexing)
//The default pattern is 32 bits wide and 32 bits tall, or 4 bytes per row and 32 tall, 4 * 32 = 128 bytes.
#define TGL_POLYGON_STIPPLE_MASK_X 31
#define TGL_POLYGON_STIPPLE_MASK_Y 31



//Prevent ZB_copyFrameBuffer from copying certain colors.
#define TGL_FEATURE_NO_COPY_COLOR 0
//Don't draw (texture mapped) pixels whose color is the NO_DRAW_COLOR
#define TGL_FEATURE_NO_DRAW_COLOR 0
//Regardless of the current clear color, always clear using the NO_COPY_COLOR
#define TGL_FEATURE_FORCE_CLEAR_NO_COPY_COLOR 0
#define TGL_NO_COPY_COLOR 0xff00ff
#define TGL_NO_DRAW_COLOR 0xff00ff
//^ solid debug pink.
#define TGL_COLOR_MASK 0x00ffffff
//^ mask to check for while drawing/copying.


//DO NOT TURN THESE ON, I don't maintain them and I actively #error them out.
#define TGL_FEATURE_8_BITS         0
#define TGL_FEATURE_24_BITS        0

//These are the only maintained modes.
#define TGL_FEATURE_16_BITS        0
#define TGL_FEATURE_32_BITS        1

//#define TGL_FEATURE_RENDER_BITS    15
//#define TGL_FEATURE_RENDER_BITS    16

#if TGL_FEATURE_32_BITS == 1
#define TGL_FEATURE_RENDER_BITS    32

#elif TGL_FEATURE_16_BITS == 1
#define TGL_FEATURE_RENDER_BITS    16

#else
#error "Unsupported TGL_FEATURE_XX_BITS"

#endif

#endif 
/* _tgl_features_h_ */
