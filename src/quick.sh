#/bin/bash
#escape character is used here to add a slash
#This script was used to move these files to include.
#sed -i 's/\"zbuffer.h/\"include\/zbuffer.h/g' *.c *.h
#sed -i 's/\"zfeatures.h/\"include\/zfeatures.h/g' *.c *.h
sed -i 's/\"include/\"..\/include/g' *.c *.h
#gcc *.c -o executable.out -lglut -lGL -lm -lGLU
