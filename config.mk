#####################################################################
# C compiler

CC= clang
#CC=gcc
#CC=tcc
#uncomment the following line for compiling the lib using tinyc.
#note that the demos will fail to build with tinyc, but you can 
#-link- to tinygl built with tinyc using gcc.
#CC= tcc
#CFLAGS= -Wall -w -O3 -g -std=c99 -mtune=native -DNDEBUG
#CFLAGS= -Wall -w -O3 -g -std=c99 -march=native -DNDEBUG

#extract the maximum possible performance from gcc.
CFLAGS= -Wall -Ofast -std=c99 -DNDEBUG -fopenmp -mtune=native -Wno-unused-function
CFLAGS_LIB= -Wall -Ofast -std=c89 -pedantic -DNDEBUG -fopenmp -mtune=native -Wno-unused-function
#CFLAGS= -Wall -Ofast -std=gnu99 -DNDEBUG -mtune=native
#uncomment the following line for compiling the lib with tinyc
#keep these flags for compiling with gcc if you are linking with tinygl compiled with tcc.
#CFLAGS= -Wall -O3 -std=gnu99 -DNDEBUG -g
#CFLAGS= -Wall -O1 -g -std=c99 -Wno-undef -DNDEBUG
LFLAGS=




