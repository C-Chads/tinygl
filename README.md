# TinyGL- New and Improved
![GIF Video of demo](capture.gif)

TinyGL 0.6 (c) 1997-2021 Fabrice Bellard, C-Chads, Gek (see License, it's free software)

This is a maintained fork of TinyGL, by the C-Chads.
It is a small, suckless Software-only partial GL 1.1 implementation.

The original project was by Fabrice Bellard. We have forked it.
The changelog is as such:
* Disabled all non-RGBA rendering modes. The only supported mode is now RGBA.

NEW FUNCTIONS

# glSetEnableSpecular(int shouldenablespecular);

	Allows you to configure specular rendering. Turn it off
	if you want to use GL_LIGHTING but don't plan on using
	specular lighting. it will save cycles.

# glGetTexturePixmap(int text, int level, int* xsize, int* ysize)

	Allows the user to retrieve the raw pixel data of a texture, for their own modification.

# glDrawText(const unsigned char* text, int x, int y, unsigned int pixel)

	Draws a pre-made 8x8 font to the screen. You can change its displayed size with...

# glTextSize(GLTEXTSIZE mode)

	Set size of text drawn to the buffer in aforementioned function.

# glPlotPixel(int x, int y, unsigned int pixel)

	Plot pixel directly to the buffer.


FIXED BUGS FROM THE ORIGINAL!
  TinyGL 0.4 had incorrect color interpolation and issues with
  GL_FLAT, causing the hello world triangle to look rather...
  wrong. Additionally, per vertex color is just cool.

  It's fixed.
FULLY COMPATIBLE WITH RGBA!
  The library is now configured properly for RGBA rendering.

General Description:
--------------------

TinyGL is intended to be a very small implementation of a subset of
OpenGL* for embedded systems or games. It is a software only
implementation. Only the main OpenGL calls are implemented. All the
calls I considered not important are simply *not implemented*.

The main strength of TinyGL is that it is fast and simple because it
has not to be exactly compatible with OpenGL. In particular, the
texture mapping and the geometrical transformations are very fast.

The main features of TinyGL are:

- Header compatible with OpenGL (the headers are adapted from the very good
Mesa by Brian Paul et al.)

- Zlib-like licence for easy integration in commercial designs (read
the LICENCE file).

- Subset of GLX for easy testing with X Window. 

- GLX like API (NGLX) to use it with NanoX in MicroWindows/NanoX.

- Subset of BGLView under BeOS.

- OpenGL like lightening.

- Complete OpenGL selection mode handling for object picking.

- 16 bit Z buffer. 16/24/32 bit RGB rendering. High speed dithering to
paletted 8 bits if needed. High speed conversion to 24 bit packed
pixel or 32 bit RGBA if needed.

- Fast Gouraud shadding optimized for 16 bit RGB.

- Fast texture mapping capabilities, with perspective correction and
texture objects.

- 32 bit float only arithmetic.

- Very small: compiled code size of about 40 kB on x86. The file
  src/zfeatures.h can be used to remove some unused features from
  TinyGL.

- C sources for GCC on 32/64 bit architectures. It has been tested
succesfully on x86-Linux and sparc-Solaris.

Examples:
---------

I took three simple examples from the Mesa package to test the main
functions of TinyGL. You can link them to either TinyGL, Mesa or any
other OpenGL/GLX implementation. You can also compile them with
Microwindows.

- texobj illustrates the use of texture objects. Its shows the speed
of TinyGL in this case.

- glutmech comes from the GLUT packages. It is much bigger and slower
because it uses the lightening. I have just included some GLU
functions and suppressed the GLUT related code to make it work. It
shows the display list handling of TinyGL in particular. You can look
at the source code to learn the keys to move the robot. The key 't'
toggles between shaded rendering and wire frame.

- You can download and compile the VReng project to see that TinyGL
has been successfully used in a big project
(http://www-inf.enst.fr/vreng).

Architecture:
-------------

TinyGL is made up four main modules:

- Mathematical routines (zmath).

- OpenGL-like emulation (zgl).

- Z buffer and rasterisation (zbuffer).

- GLX interface (zglx).

To use TinyGL in an embedded system, you should look at the GLX layer
and modify it to suit your need. Adding a more user friendly
developper layer (as in Mesa) may be useful.

Notes - limitations:
--------------------

- See the file 'LIMITATIONS' to see the current functions supported by the API.

- The multithreading could be easily implemented since no global state
is maintainted. The library gets the current context with a function
which can be modified.

- The lightening is not very fast. I supposed that in most games the
lightening is computed by the 3D engine.

- Some changes are needed for 64 bit pointers for the handling of
arrays of float with the GLParam union.

- List sharing is partialy supported in the source, but not by the
current TinyGLX implementation (is it really useful ?).

- No user clipping planes are supported.

- No color index mode (no longer useful !)

- The mipmapping is not implemented.

- The perspecture correction in the mapping code does not use W but
1/Z. In any 'normal scene' it should work.

- The resizing of the viewport in TinyGLX ensures that the width and
the height are multiples of 4. This is not optimal because some pixels
of the window may not be refreshed.

Why ?
-----

TinyGL was developped as a student project for a Virtual Reality
network system called VReng (see the VReng home page at
http://www-inf.enst.fr/vreng).

At that time (January 1997), my initial project was to write my own 3D
rasterizer based on some old sources I wrote. But I realized that it
would be better to use OpenGL to work on any platform. My problem was
that I wanted to use texture mapping which was (and is still) quite
slower on many software OpenGL implementation. I could have modified
Mesa to suit my needs, but I really wanted to use my old sources for
that project. 

I finally decided to use the same syntax as OpenGL but with my own
libraries, thinking that later it could ease the porting of VReng to
OpenGL.

Now VReng is at last compatible with OpenGL, and I managed to patch
TinyGL so that VReng can still work with it without any modifications.

Since TinyGL may be useful for some people, especially in the world of
embedded designs, I decided to release it 'as is', otherwise, it would
have been lost on my hard disk !

------------------------------------------------------------------------------
* OpenGL(R) is a registered trademark of Silicon Graphics, Inc.
------------------------------------------------------------------------------
Fabrice Bellard.
