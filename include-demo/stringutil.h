#ifndef STRUTIL_H
#define STRUTIL_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
//Before we get on, "stringutil.h" is the most C-ish name for a source code file ever, amirite?

#ifndef STRUTIL_ALLOC
#define STRUTIL_ALLOC(s) malloc(s)
#endif

#ifndef STRUTIL_FREE
#define STRUTIL_FREE(s) free(s)
#endif

#ifndef STRUTIL_REALLOC
#define STRUTIL_REALLOC(s, t) realloc(s,t)
#endif

#ifndef STRUTIL_NO_SHORT_NAMES
#define strcata strcatalloc
#define strcataf1 strcatallocf1
#define strcataf2 strcatallocf2
#define strcatafb strcatallocfb
#endif

//Strcat but with malloc.
static inline char* strcatalloc(const char* s1, const char* s2){
	char* d = NULL; d = STRUTIL_ALLOC(strlen(s1) + strlen(s2) + 1);
	if(d){
		strcpy(d, s1);
		strcat(d, s2);
	}
	return d;
}
//Free the first argument.
static inline char* strcatallocf1(char* s1, const char* s2){
	char* d = STRUTIL_REALLOC(s1, strlen(s1) + strlen(s2) + 1);
	//char* d = NULL; d = STRUTIL_ALLOC(strlen(s1) + strlen(s2) + 1);
	if(d){
		//strcpy(d, s1);
		strcat(d, s2);
	}
	//STRUTIL_FREE(s1);
	return d;
}
//Free the second argument.
static inline char* strcatallocf2(const char* s1, char* s2){
	char* d = NULL; d = STRUTIL_ALLOC(strlen(s1) + strlen(s2) + 1);
	if(d){
		strcpy(d, s1);
		strcat(d, s2);
	}
	STRUTIL_FREE(s2);
	return d;
}
//Free both arguments
static inline char* strcatallocfb(char* s1, char* s2){
	char* d = NULL; d = STRUTIL_ALLOC(strlen(s1) + strlen(s2) + 1);
	if(d){
		strcpy(d, s1);
		strcat(d, s2);
	}
	STRUTIL_FREE(s1);
	STRUTIL_FREE(s2);
	return d;
}

//Convert a non-null-terminated URL into a null terminated one.
static inline char* str_null_terminated_alloc(const char* in, unsigned int len){
	char* d = NULL; d = malloc(len+1);
	if(d){
		memcpy(d,in,len);
		d[len] = '\0';
	}
	return d;
}

static inline unsigned int strprefix(const char *pre, const char *str)
{
    size_t lenpre = strlen(pre),
           lenstr = strlen(str);
    return lenstr < lenpre ? 0 : memcmp(pre, str, lenpre) == 0;
}

static inline unsigned int streq(const char *pre, const char *str)
{
    return strcmp(pre, str) == 0;
}
//Someone once said sub-string search was an O(n^2) algorithm. What the hell?
static inline long long strfind(const char* text, const char* subtext){
	long long ti = 0;
	long long si = 0;
	long long st = strlen(subtext);
	for(;text[ti] != '\0';ti++){
		if(text[ti] == subtext[si]) {
			si++; 
			if(subtext[si] == '\0') return (ti - st)+1;
		}else {
			si = 0;
			if(text[ti] == subtext[si])si++; //Minor case.
			if(subtext[si] == '\0') return (ti - st);
		}
	}
	return -1;
}

//Read file until terminator character is found.
//Returns the number of characters copied.
static inline unsigned long long read_until_terminator(FILE* f, char* buf, const unsigned long long buflen, char terminator){
	unsigned long long i = 0;
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

//Same as above but allocates memory to guarantee it can hold the entire thing. Grows naturally.
static inline char* read_until_terminator_alloced(FILE* f, unsigned long long* lenout, char terminator, unsigned long long initsize){
	char c;
	char* buf = STRUTIL_ALLOC(initsize);
	if(!buf) return NULL;
	unsigned long long bcap = initsize;
	unsigned long long blen = 0;
	while(1){
		if(feof(f)){break;}
		c = fgetc(f);
		if(c == terminator) {break;}
		if(blen == (bcap-1))	//Grow the buffer.
			{
				bcap<<=1;
				char* bufold = buf;
				buf = STRUTIL_REALLOC(buf, bcap);
				if(!buf){free(bufold); return NULL;}
			}
		buf[blen++] = c;
	}
	buf[blen] = '\0'; //READ_UNTIL_TERMINATOR ALWAYS RETURNS A VALID STRING!
	*lenout = blen;
	return buf;
}


static inline void* read_file_into_alloced_buffer(FILE* f, unsigned long long* len){
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

//GEK'S SIMPLE TEXT COMPRESSION SCHEMA

/*LIMITATIONS
* Token names must be alphabetic (a-z, A-Z)
* The token mark must be escaped with a backslash.
* Token names which are substrings of other ones must be listed later
*/
static inline char* strencodealloc(const char* inbuf, const char** tokens, unsigned long long ntokens, char esc, char tokmark){
	unsigned long long lenin = strlen(inbuf);
	char c; unsigned long long i = 0;
	char c_str[512] = {0}; //We are going to be sprintf-ing to this buffer.
	char* out = NULL;
	c_str[0] = esc;
	c_str[1] = tokmark;
	out = strcatalloc(c_str, "");
	c_str[0] = 0;
	c_str[1] = 0;
	//Write out all the token entries. format is namelength~definition
	for(unsigned long long j = 0; j < ntokens; j++){
		out = strcataf1(out, tokens[2*j]);
		//Write out the length of the token.
		snprintf(c_str, 512, "%llu", (unsigned long long)strlen(tokens[2*j+1]));
		out = strcataf1(out, c_str);
		c_str[0] = tokmark;
		c_str[1] = 0;
		out = strcataf1(out, c_str);
		out = strcataf1(out, tokens[2*j+1]);	
	}
	c_str[0] = esc;
	c_str[1] = 0;
	out = strcataf1(out, c_str);
	//We have now created the header. Now to begin encoding the text.
	for(i=0; i<lenin; i++){
		for(unsigned long long t = 0; t < ntokens; t++) //t- the token we are processing.
			if(strprefix(tokens[t*2+1], inbuf+i)){ //Matched at least one
				unsigned long long howmany = 1;
				unsigned long long curtoklen = strlen(tokens[t*2+1]); //Length of the current token we are counting
				for(unsigned long long h=1;i+h*curtoklen < lenin;h++){
					if(strprefix(tokens[t*2+1], inbuf+i+h*curtoklen))
						{howmany++;}
					else
						break; //The number of these things is limited.
				}
				//We know what token and how many, write it to out
				
				c_str[0] = tokmark;
				c_str[1] = 0;
				out = strcataf1(out, c_str);
				if(howmany > 1){
					snprintf(c_str, 512, "%llu", (unsigned long long)howmany);
					out = strcataf1(out, c_str);
				}
				out = strcataf1(out, tokens[t*2]);
				i+=howmany*curtoklen;
				continue;
			}
		//Test if we need to escape a sequence.
		if(inbuf[i] == esc || inbuf[i] == tokmark){
			c_str[0] = esc;
			c_str[1] = 0;
			out = strcataf1(out, c_str);
		}
		//We were unable to find a match, just write the character out.
		c_str[0] = inbuf[i];
		c_str[1] = 0;
		out = strcataf1(out, c_str);
	}
	return out;
}

static inline char* strdecodealloc(char* inbuf){
	unsigned long long lenin = strlen(inbuf);
	if(lenin < 3) {
		//puts("\nToo Short!\n");
		return NULL;
	}
	char esc = inbuf[0]; //The escape character is the first one.
	char tokmark = inbuf[1]; //Begin token character.
	//printf("Escape is %c, tokmark is %c\n", esc, tokmark);
	char c; unsigned long long i = 2;
	char c_str[2] = {0,0};
	//Our decoded text.
	char* out = strcatalloc("","");
	//Tokens for replacement, even is the token,
	//odd is its definition
	char** tokens = NULL;
	//unsigned long long* toklens = NULL;
	unsigned long long ntokens = 0;
//#define {if(i <= lenin) c = inbuf[i++]; else {goto end;}} {if(i <= lenin) c = inbuf[i++]; else {goto end;}}
	//Retrieve the tokens.
	{if(i <= lenin) c = inbuf[i++]; else {goto end;}}; //has to occur before the loop.
	while(c != esc){	ntokens++;
		tokens = STRUTIL_REALLOC(tokens, ntokens * 2 * sizeof(char*)); 
		//toklens = STRUTIL_REALLOC(toklens, ntokens * sizeof(unsigned long long));
		//toklens[ntokens-1] = 0;
		tokens[(ntokens-1)*2] = strcatalloc("","");
		tokens[(ntokens-1)*2+1] = strcatalloc("","");
		//name of token is tokens[(ntokens-1)*2] and its definition is tokens[(ntokens-1)*2+1]
		//Get the name of the token.
		if(!isalpha(c)) goto end;	//Error! Can't have Break out.
		while(isalpha(c)){
			c_str[0] = c;
			tokens[(ntokens-1)*2] = strcatallocf1(tokens[(ntokens-1)*2], c_str);
			{if(i <= lenin) c = inbuf[i++]; else {goto end;}};
		}
		//The last retrieve() got us the first digit of the token length.
		//Get the length of the token
		unsigned long long l = 0;
		if(!isdigit(c)) goto end;
		while(isdigit(c) && c!=tokmark){
			c_str[0] = c;
			l *= 10;
			l += atoi(c_str);
			{if(i <= lenin) c = inbuf[i++]; else {goto end;}};
		}
		//toklens[ntokens-1] = l;
		//We have the name of the token and its length, the last {if(i <= lenin) c = inbuf[i++]; else {goto end;}} got us the token character (~ in my example)
		//Now we can grab the token definition.
		for(unsigned long long vv = 0; vv < l; vv++){
			{if(i <= lenin) c = inbuf[i++]; else {goto end;}};
			c_str[0] = c;
			tokens[(ntokens-1)*2+1] = strcatallocf1(tokens[(ntokens-1)*2+1], c_str);	
		}
		{if(i <= lenin) c = inbuf[i++]; else {goto end;}};
	//	printf("\nTOKEN %s IS %s, length %llu",tokens[(ntokens-1)*2] ,tokens[(ntokens-1)*2+1], l);
	}
	//puts("\nREACHED ESCAPE CHARACTER.");
	//Now we attempt to build our string
	{if(i <= lenin) c = inbuf[i++]; else {goto end;}};
	long long doescape = 0;
	while(i<=lenin){
		if(!doescape && c==esc){
			doescape=1;{if(i <= lenin) c = inbuf[i++]; else {goto end;}};continue;
		}
		if(!doescape && c==tokmark){
			//Handle digits prefixing a token.
			unsigned long long l = 0;
			{if(i <= lenin) c = inbuf[i++]; else {goto end;}};
			if(isdigit(c))
				while(isdigit(c)){
					c_str[0] = c;
					l *= 10;
					l += atoi(c_str);
					{if(i <= lenin) c = inbuf[i++]; else {goto end;}};
				}
			else {l=1;}
			i--;
			
			for(unsigned long long t = 0; t < ntokens; t++)
				if(strprefix(tokens[t*2], inbuf+i)){
					//MATCH!
					for(unsigned long long q = 0; q < l; q++)
						out = strcatallocf1(out, tokens[t*2+1]);
					i+=strlen(tokens[t*2]);
					break; //break out of the for.
				}
			if(i<=lenin) {if(i <= lenin) c = inbuf[i++]; else {goto end;}};
			continue;
		}else{
			c_str[0] = c;
			out = strcatallocf1(out, c_str);
			doescape = 0;
			{if(i <= lenin) c = inbuf[i++]; else {goto end;}};
		}
	}
	end:
	if(tokens){
		for(unsigned long long j = 0; j < ntokens; j++)
			{STRUTIL_FREE(tokens[j*2]);STRUTIL_FREE(tokens[j*2+1]);}
		STRUTIL_FREE(tokens);
	}
	//if(toklens)STRUTIL_FREE(toklens);
	return out;
}
//STRUTIL_H 
#endif
