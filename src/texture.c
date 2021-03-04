/*
 * Texture Manager
 */

#include "zgl.h"

static GLTexture* find_texture(GLContext* c, GLint h) {
	GLTexture* t;
	t = c->shared_state.texture_hash_table[h % TEXTURE_HASH_TABLE_SIZE];
	while (t != NULL) {
		if (t->handle == h)
			return t;
		t = t->next;
	}
	return NULL;
}

GLboolean glAreTexturesResident(	GLsizei n,
								 	const GLuint * textures,
								 	GLboolean * residences){
GLContext* c = gl_get_context();
#define RETVAL GL_FALSE
#include "error_check.h"
GLboolean retval = GL_TRUE;
	for(GLint i = 0; i < n; i++)
		if(find_texture(c, textures[i]))
			{residences[i] = GL_TRUE;}
		else
			{residences[i] = GL_FALSE;retval = GL_FALSE;}
	return retval;
}
GLboolean glIsTexture(	GLuint texture){
	GLContext* c = gl_get_context();
#define RETVAL GL_FALSE
#include "error_check.h"
	if(find_texture(c, texture))
		return GL_TRUE;
	return GL_FALSE;
}

void* glGetTexturePixmap(GLint text, GLint level, GLint* xsize, GLint* ysize) {
	GLTexture* tex;
	GLContext* c = gl_get_context();
#if TGL_FEATURE_ERROR_CHECK == 1
	if(!(text >= 0 && level < MAX_TEXTURE_LEVELS))
#define ERROR_FLAG GL_INVALID_ENUM
#define RETVAL NULL
#include "error_check.h"
#else
	//assert(text >= 0 && level < MAX_TEXTURE_LEVELS);
#endif
	tex = find_texture(c, text);
	if (!tex)
#if TGL_FEATURE_ERROR_CHECK == 1
#define ERROR_FLAG GL_INVALID_ENUM
#define RETVAL NULL
#include "error_check.h"
#else
		return NULL;
#endif
	*xsize = tex->images[level].xsize;
	*ysize = tex->images[level].ysize;
	return tex->images[level].pixmap;
}

static void free_texture(GLContext* c, GLint h) {
	GLTexture *t, **ht;
	GLImage* im;
	GLint i;

	t = find_texture(c, h);
	if (t->prev == NULL) {
		ht = &c->shared_state.texture_hash_table[t->handle % TEXTURE_HASH_TABLE_SIZE];
		*ht = t->next;
	} else {
		t->prev->next = t->next;
	}
	if (t->next != NULL)
		t->next->prev = t->prev;

	for (i = 0; i < MAX_TEXTURE_LEVELS; i++) {
		im = &t->images[i];
		if (im->pixmap != NULL)
			gl_free(im->pixmap);
	}

	gl_free(t);
}

GLTexture* alloc_texture(GLContext* c, GLint h) {
	GLTexture *t, **ht;
#define RETVAL NULL
#include "error_check.h"
	t = gl_zalloc(sizeof(GLTexture));
	if(!t)
#if TGL_FEATURE_ERROR_CHECK == 1
#define ERROR_FLAG GL_OUT_OF_MEMORY
#define RETVAL NULL
#include "error_check.h"
#else
		gl_fatal_error("GL_OUT_OF_MEMORY");
#endif

	ht = &c->shared_state.texture_hash_table[h % TEXTURE_HASH_TABLE_SIZE];

	t->next = *ht;
	t->prev = NULL;
	if (t->next != NULL)
		t->next->prev = t;
	*ht = t;

	t->handle = h;

	return t;
}

void glInitTextures(GLContext* c) {
	/* textures */

	c->texture_2d_enabled = 0;
	c->current_texture = find_texture(c, 0);
}

void glGenTextures(GLint n, GLuint* textures) {
	GLContext* c = gl_get_context();
	GLint max, i;
	GLTexture* t;
#include "error_check.h"
	max = 0;
	for (i = 0; i < TEXTURE_HASH_TABLE_SIZE; i++) {
		t = c->shared_state.texture_hash_table[i];
		while (t != NULL) {
			if (t->handle > max)
				max = t->handle;
			t = t->next;
		}
	}
	for (i = 0; i < n; i++) {
		textures[i] = max + i + 1; //MARK: How texture handles are created.
	}
}

void glDeleteTextures(GLint n, const GLuint* textures) {
	GLContext* c = gl_get_context();
	GLint i;
	GLTexture* t;
#include "error_check.h"
	for (i = 0; i < n; i++) {
		t = find_texture(c, textures[i]);
		if (t != NULL && t != 0) {
			if (t == c->current_texture) {
				glBindTexture(GL_TEXTURE_2D, 0);
#include "error_check.h"
			}
			free_texture(c, textures[i]);
		}
	}
}

void glopBindTexture(GLContext* c, GLParam* p) {
	GLint target = p[1].i;
	GLint texture = p[2].i;
	GLTexture* t;
#if TGL_FEATURE_ERROR_CHECK == 1
	if(!(target == GL_TEXTURE_2D && target > 0))
#define ERROR_FLAG GL_INVALID_ENUM
#include "error_check.h"
#else
	//assert(target == GL_TEXTURE_2D && target > 0);
#endif
	t = find_texture(c, texture);
	if (t == NULL) {
		t = alloc_texture(c, texture);
#include "error_check.h"
	}
	if(t == NULL) { //Failed malloc.
#if TGL_FEATURE_ERROR_CHECK == 1
#define ERROR_FLAG GL_OUT_OF_MEMORY
#include "error_check.h"
#else
		gl_fatal_error("GL_OUT_OF_MEMORY");
#endif
	}
	c->current_texture = t;
}
//



//TODO: Write this, then
//Write something to test this. This function is useful for doing render targets in TinyGL
//- not that you couldn't do that
//already by manually copying pixels around. But, this is a nifty utility, eh?
void glCopyTexImage2D(	
	GLenum target, //1
 	GLint level,//2
 	GLenum internalformat, //3
 	GLint x,//4
 	GLint y,//5
 	GLsizei width,//6
 	GLsizei height,
 	GLint border)
{
	GLContext* c = gl_get_context();
#include "error_check.h"
	GLParam p[9];
	p[0].op = OP_CopyTexImage2D;
	p[1].i = target;
	p[2].i = level;
	p[3].i = internalformat;
	p[4].i = x;
	p[5].i = y;
	p[6].i = width;
	p[7].i = height;
	p[8].i = border;
	gl_add_op(p);
}
void glopCopyTexImage2D(GLContext* c, GLParam* p){
	GLint target = p[1].i;
	GLint level = p[2].i;
	//GLenum internalformat = p[3].i;
	GLint x = p[4].i;
	GLint y = p[5].i;
	GLsizei w = p[6].i;
	GLsizei h = p[7].i;
	y -= h; //Spec says LOWER left corner. So, let's change it to top left, for our purposes, eh?
	GLint border = p[8].i;
	//todo
	if(c->readbuffer != GL_FRONT ||
		c->current_texture == NULL ||
		target != GL_TEXTURE_2D ||
		border != 0 ||
		w != TGL_FEATURE_TEXTURE_DIM ||  //TODO Implement image interp
		h != TGL_FEATURE_TEXTURE_DIM
	)
	{
#if TGL_FEATURE_ERROR_CHECK == 1
#define ERROR_FLAG GL_INVALID_OPERATION
#include "error_check.h"	
#else
		return;
#endif
	}
	PIXEL* data = gl_malloc(TGL_FEATURE_TEXTURE_DIM * TGL_FEATURE_TEXTURE_DIM * sizeof(PIXEL)); //GUARDED
	if(!data){
#if TGL_FEATURE_ERROR_CHECK == 1
#define ERROR_FLAG GL_OUT_OF_MEMORY
#include "error_check.h"
#else
		gl_fatal_error("GL_OUT_OF_MEMORY");
#endif
	}
	//sample the buffer.
	//TODO implement the scaling and stuff that the GL spec says it should have.
	for(GLint j = 0; j < h; j++)
	for(GLint i = 0; i < w; i++){
		/*
		GLfloat dx = (GLfloat)i/(GLfloat)w;
		GLfloat dy = (GLfloat)j/(GLfloat)h;
		dx *= TGL_FEATURE_TEXTURE_DIM;
		dy *= TGL_FEATURE_TEXTURE_DIM;
		GLuint xdest = (dx<0)?0:((dx>TGL_FEATURE_TEXTURE_DIM-1)?TGL_FEATURE_TEXTURE_DIM-1:dx);
		GLuint ydest = (dy<0)?0:((dy>TGL_FEATURE_TEXTURE_DIM-1)?TGL_FEATURE_TEXTURE_DIM-1:dy);
		*/
		data[i+j*w] = c->zb->pbuf[		((i+x)%(c->zb->xsize))
									+	((j+y)%(c->zb->ysize))*(c->zb->xsize)];
	}
	//TODO: Load this into a texture.
	GLImage* im = &c->current_texture->images[level];
	im->xsize = TGL_FEATURE_TEXTURE_DIM;
	im->ysize = TGL_FEATURE_TEXTURE_DIM;
	if (im->pixmap != NULL) gl_free(im->pixmap);
	im->pixmap = data;
	//if(data)gl_free(data);
}

void glopTexImage1D(GLContext* c, GLParam* p){
	GLint target = p[1].i;
	GLint level = p[2].i;
	GLint components = p[3].i;
	GLint width = p[4].i;
	//GLint height = p[5].i;
	GLint height = 1;
	GLint border = p[5].i;
	GLint format = p[6].i;
	GLint type = p[7].i;
	void* pixels = p[8].p;
	GLImage* im;
	GLubyte* pixels1;
	GLint do_free;

	{
#if TGL_FEATURE_ERROR_CHECK == 1
	if (!(c->current_texture != NULL && target == GL_TEXTURE_1D && level == 0 && components == 3 && border == 0 && format == GL_RGB && type == GL_UNSIGNED_BYTE))
#define ERROR_FLAG GL_INVALID_ENUM
#include "error_check.h"

#else
	if (!(c->current_texture != NULL && target == GL_TEXTURE_1D && level == 0 && components == 3 && border == 0 && format == GL_RGB && type == GL_UNSIGNED_BYTE))
		gl_fatal_error("glTexImage2D: combination of parameters not handled!!");
#endif
	}
	if (width != TGL_FEATURE_TEXTURE_DIM || height != TGL_FEATURE_TEXTURE_DIM) {
		pixels1 = gl_malloc(TGL_FEATURE_TEXTURE_DIM * TGL_FEATURE_TEXTURE_DIM * 3);//GUARDED
		if(pixels1 == NULL){
#if TGL_FEATURE_ERROR_CHECK == 1
#define ERROR_FLAG GL_OUT_OF_MEMORY
#include "error_check.h"
#else
			gl_fatal_error("GL_OUT_OF_MEMORY");
#endif
		}
		/* no GLinterpolation is done here to respect the original image aliasing ! */
		//TODO: Make this more efficient.
		gl_resizeImageNoInterpolate(pixels1, TGL_FEATURE_TEXTURE_DIM, TGL_FEATURE_TEXTURE_DIM, pixels, width, height);
		do_free = 1;
		width = TGL_FEATURE_TEXTURE_DIM;
		height = TGL_FEATURE_TEXTURE_DIM; //TODO: make this more efficient.
	} else {
		pixels1 = pixels;
	}


	im = &c->current_texture->images[level];
	im->xsize = width;
	im->ysize = height;
	if (im->pixmap != NULL) gl_free(im->pixmap);
#if TGL_FEATURE_RENDER_BITS == 32
	im->pixmap = gl_malloc(width * height * 4); //GUARDED
	if (im->pixmap) {
		gl_convertRGB_to_8A8R8G8B(im->pixmap, pixels1, width, height);
	}else { //failed malloc of im->pixmap, glopteximage1d
#if TGL_FEATURE_ERROR_CHECK == 1
#define ERROR_FLAG GL_OUT_OF_MEMORY
#include "error_check.h"
#else
		gl_fatal_error("GL_OUT_OF_MEMORY");
#endif
	}
#elif TGL_FEATURE_RENDER_BITS == 16
	im->pixmap = gl_malloc(width * height * 2);//GUARDED
	if (im->pixmap) {
		gl_convertRGB_to_5R6G5B(im->pixmap, pixels1, width, height);
	}else {
#if TGL_FEATURE_ERROR_CHECK == 1
#define ERROR_FLAG GL_OUT_OF_MEMORY
#include "error_check.h"
#else
		gl_fatal_error("GL_OUT_OF_MEMORY");
#endif
	}

#else
#error TODO
#endif
	if (do_free)
		gl_free(pixels1);





}
void glopTexImage2D(GLContext* c, GLParam* p) {
	GLint target = p[1].i;
	GLint level = p[2].i;
	GLint components = p[3].i;
	GLint width = p[4].i;
	GLint height = p[5].i;
	GLint border = p[6].i;
	GLint format = p[7].i;
	GLint type = p[8].i;
	void* pixels = p[9].p;
	GLImage* im;
	GLubyte* pixels1;
	GLint do_free;

	 {
#if TGL_FEATURE_ERROR_CHECK == 1
	if (!(c->current_texture != NULL && target == GL_TEXTURE_2D && level == 0 && components == 3 && border == 0 && format == GL_RGB && type == GL_UNSIGNED_BYTE))
#define ERROR_FLAG GL_INVALID_ENUM
#include "error_check.h"

#else
	if (!(c->current_texture != NULL && target == GL_TEXTURE_2D && level == 0 && components == 3 && border == 0 && format == GL_RGB && type == GL_UNSIGNED_BYTE))
		gl_fatal_error("glTexImage2D: combination of parameters not handled!!");
#endif
	}

	do_free = 0;
	if (width != TGL_FEATURE_TEXTURE_DIM || height != TGL_FEATURE_TEXTURE_DIM) {
		pixels1 = gl_malloc(TGL_FEATURE_TEXTURE_DIM * TGL_FEATURE_TEXTURE_DIM * 3);//GUARDED
		if(pixels1 == NULL){
#if TGL_FEATURE_ERROR_CHECK == 1
#define ERROR_FLAG GL_OUT_OF_MEMORY
#include "error_check.h"
#else
			gl_fatal_error("GL_OUT_OF_MEMORY");
#endif
		}
		/* no GLinterpolation is done here to respect the original image aliasing ! */
		//printf("\nYes this is being called.");
		gl_resizeImageNoInterpolate(pixels1, TGL_FEATURE_TEXTURE_DIM, TGL_FEATURE_TEXTURE_DIM, pixels, width, height);
		do_free = 1;
		width = TGL_FEATURE_TEXTURE_DIM;
		height = TGL_FEATURE_TEXTURE_DIM;
	} else {
		pixels1 = pixels;
	}

	im = &c->current_texture->images[level];
	im->xsize = width;
	im->ysize = height;
	if (im->pixmap != NULL) gl_free(im->pixmap);
#if TGL_FEATURE_RENDER_BITS == 32
	im->pixmap = gl_malloc(width * height * 4);//GUARDED
	if (im->pixmap) {
		gl_convertRGB_to_8A8R8G8B(im->pixmap, pixels1, width, height);
	}else {
#if TGL_FEATURE_ERROR_CHECK == 1
#define ERROR_FLAG GL_OUT_OF_MEMORY
#include "error_check.h"
#else
		gl_fatal_error("GL_OUT_OF_MEMORY");
#endif
	}
#elif TGL_FEATURE_RENDER_BITS == 16
	im->pixmap = gl_malloc(width * height * 2);//GUARDED
	if (im->pixmap) {
		gl_convertRGB_to_5R6G5B(im->pixmap, pixels1, width, height);
	}else {
#if TGL_FEATURE_ERROR_CHECK == 1
#define ERROR_FLAG GL_OUT_OF_MEMORY
#include "error_check.h"
#else
		gl_fatal_error("GL_OUT_OF_MEMORY");
#endif
	}

#else
#error TODO
#endif
	if (do_free)
		gl_free(pixels1);
}

/* TODO: not all tests are done */
/*
void glopTexEnv(GLContext* c, GLParam* p) {
	GLint target = p[1].i;
	GLint pname = p[2].i;
	GLint param = p[3].i;

	if (target != GL_TEXTURE_ENV) {

	error:
#if TGL_FEATURE_ERROR_CHECK == 1

#define ERROR_FLAG GL_INVALID_ENUM
#include "error_check.h"
#else
		gl_fatal_error("glTexParameter: unsupported option");
#endif
		
	}

	if (pname != GL_TEXTURE_ENV_MODE)
		goto error;

	if (param != GL_DECAL)
		goto error;
}
*/
/* TODO: not all tests are done */
/*
void glopTexParameter(GLContext* c, GLParam* p) {
	GLint target = p[1].i;
	GLint pname = p[2].i;
	GLint param = p[3].i;

	if (target != GL_TEXTURE_2D &&
		target != GL_TEXTURE_1D) {
	error:
		tgl_warning("glTexParameter: unsupported option");
		return;
	}

	switch (pname) {
	case GL_TEXTURE_WRAP_S:
	case GL_TEXTURE_WRAP_T:
		if (param != GL_REPEAT)
			goto error;
		break;
	}
}
*/
//TODO: implement pixel packing?
/*
void glopPixelStore(GLContext* c, GLParam* p) {
	GLint pname = p[1].i;
	GLint param = p[2].i;

	if (pname != GL_UNPACK_ALIGNMENT || param != 1) {
		gl_fatal_error("glPixelStore: unsupported option");
	}
}
*/
