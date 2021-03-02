#include <string.h>
#include <stdlib.h>
#include <stdio.h>

//Before we get on, "stringutil.h" is the most C-ish name for a source code file ever, amirite?

#ifndef STRUTIL_ALLOC
#define STRUTIL_ALLOC(s) malloc(s)
#endif

#ifndef STRUTIL_NO_SHORT_NAMES
#define strcata strcatalloc
#define strcataf1 strcatallocf1
#define strcataf2 strcatallocf2
#define strcatafb strcatallocfb
#endif
//Strcat but with malloc.
static inline char* strcatalloc(const char* s1, const char* s2){
	char* d = STRUTIL_ALLOC(strlen(s1) + strlen(s2) + 1);
	strcpy(d, s1);
	strcat(d, s2);
	return d;
}

static inline char* strcatallocf1(char* s1, const char* s2){
	char* d = STRUTIL_ALLOC(strlen(s1) + strlen(s2) + 1);
	strcpy(d, s1);
	strcat(d, s2);
	free(s1);
	return d;
}

static inline char* strcatallocf2(const char* s1, char* s2){
	char* d = STRUTIL_ALLOC(strlen(s1) + strlen(s2) + 1);
	strcpy(d, s1);
	strcat(d, s2);
	free(s2);
	return d;
}

static inline char* strcatallocfb(char* s1, char* s2){
	char* d = STRUTIL_ALLOC(strlen(s1) + strlen(s2) + 1);
	strcpy(d, s1);
	strcat(d, s2);
	free(s1);
	free(s2);
	return d;
}

//Convert a non-null-terminated URL into a null terminated one.
static inline char* str_null_terminated_alloc(const char* in, unsigned int len){
	char* d = malloc(len+1);
	memcpy(d,in,len);
	d[len] = '\0';
	return d;
}

static inline unsigned int strprefix(const char *pre, const char *str)
{
    size_t lenpre = strlen(pre),
           lenstr = strlen(str);
    return lenstr < lenpre ? 0 : memcmp(pre, str, lenpre) == 0;
}

//Read file until terminator character is found.
//Returns the number of characters copied.
static inline unsigned int read_until_terminator(FILE* f, char* buf, const unsigned int buflen, char terminator){
	unsigned int i = 0;
	char c;
	for(i = 0; i < (buflen-1); i++)
	{
		if(feof(f))break;
		c = fgetc(f);
		if(c == terminator)break;
		buf[i] = c;
	}
	buf[buflen-1] = '\0'; //READ_UNTIL_TERMINATOR ALWAYS RETURNS A VALID STRING!
	return i;
}


static inline void* read_file_into_alloced_buffer(FILE* f, unsigned int* len){
	void* buf = NULL;
	if(!f) return NULL;
	fseek(f, 0, SEEK_END);
	*len = ftell(f);
	fseek(f,0,SEEK_SET);
	buf = STRUTIL_ALLOC(*len + 1);
	if(!buf) return NULL;
	fread(buf, 1, *len, f);
	((char*)buf)[*len] = '\0';
	return buf;
}
