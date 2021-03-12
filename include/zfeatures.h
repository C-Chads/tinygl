#ifndef _tgl_features_h_
#define _tgl_features_h_
/* It is possible to enable/disable (compile time) features in this
   header file. */

//Enables setting the error flags when there's an error, so you can check it with glGetError
//Disabling this has slight performance gains.
#define TGL_FEATURE_ERROR_CHECK 0
//Strict out-of-memory checking. All OpenGL function calls are invalidated (ALL OF THEM) if a GL_OUT_OF_MEMORY error occurs.
//This means that TinyGL has to constantly check all gl_malloc() attempts for errors and the state of the error state variable.
//The checks slow down the renderer so it is not recommended , but
//it's in the GL spec that this should occur.
#define TGL_FEATURE_STRICT_OOM_CHECKS 0

//Swap between using the inline'd malloc(), calloc(), and free() in zbuffer.h, or
//a replacement gl_malloc(), gl_zalloc(), and gl_free() in memory.c
#define TGL_FEATURE_CUSTOM_MALLOC 0

//Use Fast Inverse Square Root. Toggleable because it's actually slower,
//And because some systems may have float types which are incompatible with it.
#define TGL_FEATURE_FISR 1
//Clientside Arrays
#define TGL_FEATURE_ARRAYS         1
//This library has a super cool opcode system based on function pointers, it's pretty fast too.
#define TGL_FEATURE_DISPLAYLISTS   1
//Lighting is expensive on texture-mapped polygons.
#define TGL_FEATURE_LIT_TEXTURES   1
//Enable the patternized "discard"-ing of pixels.
#define TGL_FEATURE_POLYGON_STIPPLE 0
//Enable the rendering of large polygons (in terms of vertex count).
//the maximum number of vertices in a polygon is defined in zgl.h
#define TGL_FEATURE_GL_POLYGON      0
//Enable GL_BLEND functionality
#define TGL_FEATURE_BLEND 			1
//Enable blending in glDrawPixels, which slows it down a lot and makes it unsuitable for a lot of things.
#define TGL_FEATURE_BLEND_DRAW_PIXELS 0
//The width of textures as a power of 2. The default is 8, or 256x256 textures.
#define TGL_FEATURE_TEXTURE_POW2	8
#define TGL_FEATURE_TEXTURE_DIM		(1<<TGL_FEATURE_TEXTURE_POW2)

//A stipple pattern is 128 bytes in size.
#define TGL_POLYGON_STIPPLE_BYTES 128
//A stipple pattern is 2^5 (32) bits wide.
#define TGL_POLYGON_STIPPLE_POW2_WIDTH 5
//The stipple pattern mask (the last bits of the screen coordinates used for indexing)
//The default pattern is 32 bits wide and 32 bits tall, or 4 bytes per row and 32 tall, 4 * 32 = 128 bytes.
#define TGL_POLYGON_STIPPLE_MASK_X 31
#define TGL_POLYGON_STIPPLE_MASK_Y 31

//Use lookup tables for calculating specular light.
#define TGL_FEATURE_SPECULAR_BUFFERS 0

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

//MULTITHREADING OPTIONS, only applicable if compiled with -fopenmp

//Enable DrawPixels to offload the rendering of individual scanlines to separate threads.
#define TGL_FEATURE_MULTITHREADED_DRAWPIXELS	1
//Enable multithreading glCopyTexImage2D
#define TGL_FEATURE_MULTITHREADED_COPY_TEXIMAGE_2D 1
//Enable multithreading the ZB_CopyBuffer operation.
#define TGL_FEATURE_MULTITHREADED_ZB_COPYBUFFER 1
//Enable stdalign
#define TGL_FEATURE_ALIGNAS 1

#if TGL_FEATURE_ALIGNAS == 1
#include <stdalign.h>
#define TGL_ALIGN alignas(16)
#else
#define TGL_ALIGN /*a comment*/
#endif

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

//The fraction bits in the fixed point values used for S and T in interpolatiion.
#define ZB_POINT_S_FRAC_BITS 10
#define ZB_POINT_T_FRAC_BITS (ZB_POINT_S_FRAC_BITS + TGL_FEATURE_TEXTURE_POW2)

//Test the compatibility of the target platform at glInit() time.
#define TGL_FEATURE_TINYGL_RUNTIME_COMPAT_TEST 1

#define TINYGL_VERSION 0.8

#endif 
/* _tgl_features_h_ */


