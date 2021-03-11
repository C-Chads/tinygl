#####################################################################
# C compiler

CC= gcc
#CFLAGS= -Wall -w -O3 -g -std=c99 -mtune=native -DNDEBUG
#CFLAGS= -Wall -w -O3 -g -std=c99 -march=native -DNDEBUG
CFLAGS= -Wall -O3 -std=gnu99 -DNDEBUG -g -fopenmp -Wno-uninitialized
#CFLAGS= -Wall -O1 -g -std=c99 -Wno-undef -DNDEBUG
LFLAGS=




