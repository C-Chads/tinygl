#/bin/bash
#escape character is used here to add a slash
#This script was used to move these files to include.
#sed -i 's/\"zbuffer.h/\"include\/zbuffer.h/g' *.c *.h
#sed -i 's/\"zfeatures.h/\"include\/zfeatures.h/g' *.c *.h
#sed -i 's/unsigned int/GLuint/g' *.c *.h
#sed -i 's/float/GLfloat/g' *.c *.h
#sed -i 's/char/GLbyte/g' *.c *.h
#sed -i 's/unsigned GLbyte/GLubyte/g' *.c *.h
#sed -i 's/	int/	GLint/g' *.c *.h
#sed -i 's/unsigned GLshort/GLushort/g' *.c *.h
sed -i 's/\~0xAA/0x55/g' *.c

#gcc *.c -o executable.out -lglut -lGL -lm -lGLU
