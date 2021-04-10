#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/GL/gl.h"
#include "../src/font8x8_basic.h"
#include "../include-demo/stringutil.h"

char fillchar = '#';
void render(unsigned char *bitmap, int x) {
    int y;
    int set;
    for (y=0; y < 8; y++) {
       	set = bitmap[x] & (1 << y); //despite the naming convention... x is actually the row # not the column #. X IS Y, Y IS X, got it?
        fputc(set ? fillchar : ' ', stdout);
    }
}

int main(int argc, char **argv) {
	if(argc > 1) fillchar = argv[1][0];
	
	{
		unsigned long l;
		unsigned char* b = (unsigned char*)read_until_terminator_alloced(stdin, &l, '\n', 30);
		if(b)
		do {
			for(unsigned int x = 0; x < 8; x++){ //Despite the name, this is actually the row
				fputc('\n', stdout);
			    for(unsigned int i = 0; b[i] != '\n' && b[i] != '\0'; i++){
			    	unsigned char *bitmap = (unsigned char*)font8x8_basic[ b[i] ];
			    	render(bitmap, x);	
			   	}
		   	}
		//Walk b until the next \n or null terminator.
			free(b);
			b = (unsigned char*)read_until_terminator_alloced(stdin, &l, '\n', 30);
	   	} while(!feof(stdin) && b);
	   	if(b) free(b);
   	}
   	fputc('\n', stdout);
    return 0;
}
