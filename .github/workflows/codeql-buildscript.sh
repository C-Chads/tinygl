#!/usr/bin/env bash

#cd src
# inside the src directory
#gcc -O3 -c *.c 
#ar rcs libTinyGL.a *.o
# the library is now compiled
#cp libTinyGL.a ../lib
#cd ..
#cd SDL_Examples/
# build the menu demo
#gcc -O3 menu.c -o menu -lSDL ../lib/libTinyGL.a -lm
# gears
#gcc -O3 gears.c -o gears -lSDL ../lib/libTinyGL.a -lm

mkdir build && cd build && cmake .. && make
