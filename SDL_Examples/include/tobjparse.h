/* Public Domain / CC0 3d OBJ Parser

With support for Per Vertex Color (VC) Lines.


Written by Gek (DMHSW) in 2020


*/

#ifndef TOBJ_PARSE_H
#define TOBJ_PARSE_H
#include "3dMath.h"
#include "stretchy_buffer.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
typedef struct{
	long long unsigned int p;	
	long long unsigned int n;	
	long long unsigned int tc;
	long long unsigned int vc;
}facedef;
typedef struct{
	vec3* positions;
	vec3* normals;
	vec3* texcoords;
	vec3* colors;
	facedef* faces;
}objraw;
typedef struct{
	int npoints; //Number of points.
	vec3* d; //Triangles (Same winding as in the file)
	vec3* n; //Normals
	vec3* t; //Texture Cordinates
	vec3* c; //colors
}model;
objraw initobjraw(){
	return (objraw){
		.positions=NULL,
		.normals=NULL,
		.texcoords=NULL,
		.faces=NULL
	};
}
model initmodel(){
	return (model){
		.npoints=0,
		.d=NULL,
		.n=NULL,
		.t=NULL,
		.c=NULL
	};
}
void freeobjraw(objraw* o){
	sb_free(o->positions);
	sb_free(o->texcoords);
	sb_free(o->normals);
	sb_free(o->colors);
	sb_free(o->faces);
}
void freemodel(model* o){
	free(o->d);
	free(o->t);
	free(o->n);
	free(o->c);
}
model tobj_tomodel(objraw* raw){
	if(!raw || raw->faces == NULL)
	{
		puts("\nAttempted to convert empty model... Aborting...\n");
		return initmodel();
	}
	model ret = initmodel();
	ret.npoints = 0;
	ret.d=malloc(sizeof(vec3) * sb_count(raw->faces));
	if(raw->normals)ret.n=malloc(sizeof(vec3) * sb_count(raw->faces));
	if(raw->texcoords)ret.t=malloc(sizeof(vec3) * sb_count(raw->faces));
	if(raw->colors)ret.c=malloc(sizeof(vec3) * sb_count(raw->faces));
	long long unsigned int piter = 0;
	long long unsigned int niter = 0;
	long long unsigned int titer = 0;
	long long unsigned int citer = 0;
	//printf("\nsb_count of faces is %d",sb_count(raw->faces));
	for(long long int i = 0; i < sb_count(raw->faces);i++){
		//printf("\n::%lld:: 0\n",i);
		long long unsigned int p = raw->faces[i].p-1;
		long long unsigned int n = raw->faces[i].n-1;
		long long unsigned int t = raw->faces[i].tc-1;
		long long unsigned int c = raw->faces[i].vc-1;
		
		if(p < (long long unsigned int)sb_count(raw->positions)){
			//sb_push(ret.d, raw->positions[p]);
			ret.d[piter++] = raw->positions[p];
			ret.npoints++;
		} else {
			printf("\nBad Data!!!\n");
			printf("p=%llu n=%llu t=%llu c=%llu i=%lld\n\n",p,n,t,c,i);
		}
		if(raw->normals){
			if(n < (long long unsigned int)sb_count(raw->normals)){
				//sb_push(ret.n, raw->normals[n]);
				ret.n[niter++] = raw->normals[n];
			} else {
				puts("\n<BAD DATA>, NORMALS\n");
			}
		}
		if(raw->texcoords){
			if(t < (long long unsigned int)sb_count(raw->texcoords)){
				//sb_push(ret.t, raw->texcoords[t]);
				ret.t[titer++] = raw->texcoords[t];
			} else
				puts("\n<BAD DATA>, TEXCOORDS\n");
		}
		if(raw->colors){
			if(c < (long long unsigned int)sb_count(raw->colors)){
				//sb_push(ret.c, raw->colors[c]);
				ret.c[citer++] = raw->colors[c];
			} else {
				puts("\n<BAD DATA>, COLORS\n");
			}
		}
	}
	//printf("\ntobj_tomodel completed.\n");
	if(ret.npoints != piter){
		printf("\nBAD DATA!!! ABORTING...\n");
		exit(1);
	}
	return ret;
}
//Only loads 
objraw tobj_load(const char* fn){
	FILE* f;
	f = fopen(fn, "r");
	objraw retval = initobjraw();
	if(f){
		char line[2048];line[2047]=0;
		//int read = 0;


		while(fgets(line, 2047, f)){
			vec3 val;
			facedef frick0;
			facedef frick1;
			facedef frick2;
			if(line[0] == 'v' && line[1] == ' ' && (strlen(line) > 4)){
				//read = sscanf(line,"v %f %f %f",&val.d[0],&val.d[1],&val.d[2]);
				//printf("\nv Read: %d",read);
				sb_push(retval.positions, val);
				char* t = line+2;
				sb_last(retval.positions).d[0] = atof(t);
				while(!isspace(*t) && *t != '\0')t++;
				if(*t == '\0')continue;
				t++;
				sb_last(retval.positions).d[1] = atof(t);
				while(!isspace(*t) && *t != '\0')t++;
				if(*t == '\0')continue;
				t++;
				sb_last(retval.positions).d[2] = atof(t);
			}
			if(line[0] == 'v' && line[1] == 't' && (strlen(line) > 4)){
				//read = sscanf(line,"vt %f %f",&val.d[0],&val.d[1]);
				sb_push(retval.texcoords, val);
				char* t = line+3;
				sb_last(retval.texcoords).d[0] = atof(t);
				while(!isspace(*t) && *t != '\0')t++;
				if(*t == '\0')continue;
				t++;
				sb_last(retval.texcoords).d[1] = -atof(t);
				
			}
			if(line[0] == 'v' && line[1] == 'c' && (strlen(line) > 4)){
				//read=sscanf(line,"vc %f %f %f",&val.d[0],&val.d[1],&val.d[2]);
				sb_push(retval.colors, val);
				char* t = line+3;
				sb_last(retval.colors).d[0] = atof(t);
				while(!isspace(*t) && *t != '\0')t++;
				if(*t == '\0')continue;
				t++;
				sb_last(retval.colors).d[1] = atof(t);
				while(!isspace(*t) && *t != '\0')t++;
				if(*t == '\0')continue;
				t++;
				sb_last(retval.colors).d[2] = atof(t);
				//printf("\nvc Read: %d",read);
				
			}
			if(line[0] == 'v' && line[1] =='n' && (strlen(line) > 4)){
				//read=sscanf(line,"vn %f %f %f",&val.d[0],&val.d[1],&val.d[2]);
				//printf("\nn Read: %d",read);
				sb_push(retval.normals, val);
				char* t = line+3;
				sb_last(retval.normals).d[0] = atof(t);
				while(!isspace(*t) && *t != '\0')t++;
				if(*t == '\0')continue;
				t++;
				sb_last(retval.normals).d[1] = atof(t);
				while(!isspace(*t) && *t != '\0')t++;
				if(*t == '\0')continue;
				t++;
				sb_last(retval.normals).d[2] = atof(t);
				
				/*
				printf("\nreading from stretchy buffer, %f %f %f",
					retval.normals[sb_count(retval.normals)-1].d[0],
					retval.normals[sb_count(retval.normals)-1].d[1],
					retval.normals[sb_count(retval.normals)-1].d[2]
				);
				*/
			}
			if(line[0] == 'f' && (strlen(line) > 4)){
				//The face lines are hard to parse.
				//They could be p p p
				// or p/vt p/vt p/vt
				//or p//n p//n p//n
				//or p/vt/n p/vt/n p/vt/n
				//or p/vt/n/c p/vt/n/c p/vt/n/c
				//or some other combination, as long as it has p it's valid
				//Grab the position indices
				{
					char* t = line+2;
					frick0.p = strtoull(t,NULL,10);
					//printf("\nf[0].p is %llu",frick0.p);
					while(!isspace(*t) && *t != '\0')t++;
					if(*t == '\0')continue;
					t++;
					frick1.p = strtoull(t,NULL,10);
					//printf("\nf[1].p is %llu",frick1.p);
					while(!isspace(*t) && *t != '\0')t++;
					if(*t == '\0')continue;
					t++;
					frick2.p = strtoull(t,NULL,10);
					//printf("\nf[2].p is %llu",frick2.p);
				}
				//Grab the texture coordinates (First character after first slash!)
				if(retval.texcoords){
					char* t = line+2;
					while(!isspace(*t) && *t != '\0' && *t != '/')t++; //jump to first slash
					t++;//next character
					frick0.tc = strtoull(t,NULL,10);
					//printf("\nf[0].tc is %llu",frick0.tc);
					//Jump to the next group of numbers
					while(!isspace(*t) && *t != '\0')t++;
					if(*t == '\0')continue;
					t++;
					//first slash
					while(!isspace(*t) && *t != '\0' && *t != '/')t++; //jump to next slash
					t++;//next character
					frick1.tc = strtoull(t,NULL,10);
					//printf("\nf[1].tc is %llu",frick1.tc);
					//Jump to the next group of numbers
					while(!isspace(*t) && *t != '\0')t++;
					if(*t == '\0')continue;
					t++;
					//first slash
					while(!isspace(*t) && *t != '\0' && *t != '/')t++; //jump to next slash
					t++;//next character
					frick2.tc = strtoull(t,NULL,10);
					//printf("\nf[2].tc is %llu",frick2.tc);
				}
				if(retval.normals){
					char* t = line+2;
					while(!isspace(*t) && *t != '\0' && *t != '/')t++; //jump to first slash
					t++;//next character
					while(!isspace(*t) && *t != '\0' && *t != '/')t++; //jump to second slash
					t++;//next character
					frick0.n = strtoull(t,NULL,10);
					//Jump to the next group of numbers
					while(!isspace(*t) && *t != '\0')t++;
					if(*t == '\0')continue;
					t++;
					//first slash
					while(!isspace(*t) && *t != '\0' && *t != '/')t++; //jump to first slash
					t++;//next character
					while(!isspace(*t) && *t != '\0' && *t != '/')t++; //jump to second slash
					t++;//next character
					frick1.n = strtoull(t,NULL,10);

					//Jump to the next group of numbers
					while(!isspace(*t) && *t != '\0')t++;
					if(*t == '\0')continue;
					t++;
					//first slash
					while(!isspace(*t) && *t != '\0' && *t != '/')t++; //jump to first slash
					t++;//next character
					while(!isspace(*t) && *t != '\0' && *t != '/')t++; //jump to second slash
					t++;//next character
					frick2.n = strtoull(t,NULL,10);
				}
				if(retval.colors){
					char* t = line+2;
					while(!isspace(*t) && *t != '\0' && *t != '/')t++; //jump to first slash
					t++;//next character
					while(!isspace(*t) && *t != '\0' && *t != '/')t++; //jump to second slash
					t++;//next character
					while(!isspace(*t) && *t != '\0' && *t != '/')t++; //jump to third slash
					t++;//next character
					frick0.vc = strtoull(t,NULL,10);
					//printf("\nf[0].vc is %llu",frick0.vc);
					//Jump to the next group of numbers
					while(!isspace(*t) && *t != '\0')t++;
					if(*t == '\0')continue;
					t++;
					//first slash
					while(!isspace(*t) && *t != '\0' && *t != '/')t++; //jump to first slash
					t++;//next character
					while(!isspace(*t) && *t != '\0' && *t != '/')t++; //jump to second slash
					t++;//next character
					while(!isspace(*t) && *t != '\0' && *t != '/')t++; //jump to third slash
					t++;//next character
					frick1.vc = strtoull(t,NULL,10);
					//printf("\nf[1].vc is %llu",frick1.vc);
					//Jump to the next group of numbers
					while(!isspace(*t) && *t != '\0')t++;
					if(*t == '\0')continue;
					t++;
					//first slash
					while(!isspace(*t) && *t != '\0' && *t != '/')t++; //jump to first slash
					t++;//next character
					while(!isspace(*t) && *t != '\0' && *t != '/')t++; //jump to second slash
					t++;//next character
					while(!isspace(*t) && *t != '\0' && *t != '/')t++; //jump to third slash
					t++;//next character
					frick2.vc = strtoull(t,NULL,10);
					//printf("\nf[2].vc is %llu",frick2.vc);
				}
				sb_push(retval.faces,frick0);
				sb_push(retval.faces,frick1);
				sb_push(retval.faces,frick2);
				/*
				printf("\nReading from sb, frick0.p=%llu frick0.n=%llu frick0.tc=%llu frick0.vc=%llu",
					retval.faces[sb_count(retval.faces)-3].p,
					retval.faces[sb_count(retval.faces)-3].n,
					retval.faces[sb_count(retval.faces)-3].tc,
					retval.faces[sb_count(retval.faces)-3].vc
				);*/
			}
		}
	fclose(f);
	} else {
		printf("\nUnable to load file %s\n",fn);
	}
	
	return retval;
}

#endif
