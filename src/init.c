#include "msghandling.h"
#include "zgl.h"
GLContext gl_ctx;
static const GLContext empty_gl_ctx = {0};

static void initSharedState(GLContext* c) {
	GLSharedState* s = &c->shared_state;
	s->lists = gl_zalloc(sizeof(GLList*) * MAX_DISPLAY_LISTS);
	if (!s->lists)
		gl_fatal_error("TINYGL_CANNOT_INIT_OOM");
	s->texture_hash_table = gl_zalloc(sizeof(GLTexture*) * TEXTURE_HASH_TABLE_SIZE);
	if (!s->texture_hash_table)
		gl_fatal_error("TINYGL_CANNOT_INIT_OOM");
	s->buffers = gl_zalloc(sizeof(GLBuffer*) * MAX_BUFFERS);
	if (!s->buffers)
		gl_fatal_error("TINYGL_CANNOT_INIT_OOM");
	alloc_texture(0);
#include "error_check.h"
}

static void endSharedState(GLContext* c) {
	GLSharedState* s = &c->shared_state;
	GLint i;
	GLList* l;
	GLParamBuffer *pb, *pb1;
	GLTexture *t, *n;
	for (i = 0; i < MAX_DISPLAY_LISTS; i++)
		if (s->lists[i]) {
			l = s->lists[i];
			pb = l->first_op_buffer;
			while (pb != NULL) {
				pb1 = pb->next;
				gl_free(pb);
				pb = pb1;
			}
			gl_free(l);
			s->lists[i] = NULL;
		}
	gl_free(s->lists);
	for (i = 0; i < TEXTURE_HASH_TABLE_SIZE; i++) {
		t = s->texture_hash_table[i];
		while (t) {
			GLTexture** ht;
			if (t->prev == NULL) {
				ht = &c->shared_state.texture_hash_table[t->handle & TEXTURE_HASH_TABLE_MASK];
				*ht = t->next;
			} else {
				t->prev->next = t->next;
			}
			n = t->next;
			if (t->next != NULL)
				t->next->prev = t->prev;
			gl_free(t);
			t = n;
		}
	}
	gl_free(s->texture_hash_table);
	for (i = 0; i < MAX_BUFFERS; i++) {
		if (s->buffers[i]) {
			if (s->buffers[i]->data) {
				gl_free(s->buffers[i]->data);
			}
			gl_free(s->buffers[i]);
		}
	}
	gl_free(s->buffers);
}

#if TGL_FEATURE_TINYGL_RUNTIME_COMPAT_TEST == 1

#define TGL_FLOAT_ERR(a, b) ((a - b) / b)
static int TinyGLRuntimeCompatibilityTest() {
	GLfloat t = -0, tf2;
	GLint t2 = 1 << 31;
	memcpy(&tf2, &t2, 4);
	if (tf2 != t) return 1;
	t2 = 3212836864;
	t = -1;
	memcpy(&tf2, &t2, 4);
	if (tf2 != t)return 1;
	
	if (((GLint)255 << 8) != 65280)
		return 1;
	if ((GLint)65280 >> 8 != 255)
		return 1;
	if (((GLint)-1) >> 14 != -1)
		return 1;
	if (((GLuint)255 << 8) != 65280)
		return 1;
	if ((GLuint)65280 >> 8 != 255)
		return 1;
	if (((GLushort)255 << 8) != 65280)
		return 1;
	if ((GLushort)65280 >> 8 != 255)
		return 1;
	if (((GLshort)255 << 8) != 65280)
		return 1;
	if ((GLshort)65280 >> 8 != -1)
		return 1;
#if TGL_FEATURE_FISR == 1
	t = fastInvSqrt(37);
	tf2 = 1.0 / sqrt(37);
	if (TGL_FLOAT_ERR(t, tf2) > 0.05)
		return 1;
	t = fastInvSqrt(59);
	tf2 = 1.0 / sqrt(59);
	if (TGL_FLOAT_ERR(t, tf2) > 0.05)
		return 1;
	t = fastInvSqrt(1023);
	tf2 = 1.0 / sqrt(1023);
	if (TGL_FLOAT_ERR(t, tf2) > 0.05)
		return 1;

	t = fastInvSqrt(10000);
	tf2 = 1.0 / sqrt(10000);
	if (TGL_FLOAT_ERR(t, tf2) > 0.05)
		return 1;
#endif
	{
		GLint i;
		GLuint buf1[10];
		GLuint buf2[10];
		for (i = 0; i < 10; i++)
			buf1[i] = (1023 << i) + i + i % -1;
		for (i = 0; i < 10; i++)
			buf2[i] = (14 << i) + i + i % -4;
		memcpy(buf1, buf2, 10 * 4);
		for (i = 0; i < 10; i++)
			if (buf2[i] != buf1[i])
				return 1;
	}
	if (sizeof(void*) < 4)
		return 1;
	/* ZALLOC TEST*/
	{
		GLint i, j;
		for (i = 0; i < 10; i++) {
			GLubyte* data = gl_zalloc(1024); 
			if (!data)
				return 1;
			for (j = 0; j < 1024; j++)
				if (data[j] != 0)
					return 1;
			gl_free(data);
		}
	}
	return 0;
}
#endif

void glInit(void* zbuffer1) {
	GLContext* c;
	GLViewport* v;
	GLint i;
	ZBuffer* zbuffer = (ZBuffer*)zbuffer1;
#if TGL_FEATURE_TINYGL_RUNTIME_COMPAT_TEST == 1
	if (TinyGLRuntimeCompatibilityTest())
		gl_fatal_error("TINYGL_FAILED_RUNTIME_COMPAT_TEST");
#endif
	gl_ctx = empty_gl_ctx;
	c = &gl_ctx;
	if (!c)
		gl_fatal_error("TINYGL_CANNOT_INIT_OOM");

	c->zb = zbuffer;
#if TGL_FEATURE_ERROR_CHECK == 1
	c->error_flag = GL_NO_ERROR;
#endif
	/* allocate GLVertex array */
	/*c->vertex_max = POLYGON_MAX_VERTEX;*/
	/*c->vertex = gl_malloc(POLYGON_MAX_VERTEX * sizeof(GLVertex));*/
	/*if(!c->vertex) gl_fatal_error("TINYGL_CANNOT_INIT_OOM");*/
	/* viewport */
	v = &c->viewport;
	v->xmin = 0;
	v->ymin = 0;
	v->xsize = zbuffer->xsize;
	v->ysize = zbuffer->ysize;
	gl_eval_viewport();
	/* buffer stuff GL 1.1 */
	c->drawbuffer = GL_FRONT;
	c->readbuffer = GL_FRONT;
	/* shared state */
	initSharedState(c);
	/* ztext */
	c->textsize = 1;
	/* buffer */
	c->boundarraybuffer = 0;
	c->boundvertexbuffer = 0;
	c->boundcolorbuffer = 0;
	c->boundnormalbuffer = 0;
	c->boundtexcoordbuffer = 0;
	/* lists */

	c->exec_flag = 1;
	c->compile_flag = 0;
	c->print_flag = 0;
	c->listbase = 0;
	c->in_begin = 0;

	/* lights */
	for (i = 0; i < MAX_LIGHTS; i++) {
		GLLight* l = &c->lights[i];
		l->ambient = gl_V4_New(0, 0, 0, 1);
		l->diffuse = gl_V4_New(1, 1, 1, 1);
		l->specular = gl_V4_New(1, 1, 1, 1);
		l->position = gl_V4_New(0, 0, 1, 0);
		l->norm_position = gl_V3_New(0, 0, 1);
		l->spot_direction = gl_V3_New(0, 0, -1);
		l->norm_spot_direction = gl_V3_New(0, 0, -1);
		l->spot_exponent = 0;
		l->spot_cutoff = 180;
		l->attenuation[0] = 1;
		l->attenuation[1] = 0;
		l->attenuation[2] = 0;
		l->enabled = 0;
	}
	c->first_light = NULL;
	c->ambient_light_model = gl_V4_New(0.2, 0.2, 0.2, 1);
	c->local_light_model = 0;
	c->lighting_enabled = 0;
	c->light_model_two_side = 0;

	/* default materials */
	for (i = 0; i < 2; i++) {
		GLMaterial* m = &c->materials[i];
		m->emission = gl_V4_New(0, 0, 0, 1);
		m->ambient = gl_V4_New(0.2, 0.2, 0.2, 1);
		m->diffuse = gl_V4_New(0.8, 0.8, 0.8, 1);
		m->specular = gl_V4_New(0, 0, 0, 1);
		m->shininess = 0;
	}
	c->current_color_material_mode = GL_FRONT_AND_BACK;
	c->current_color_material_type = GL_AMBIENT_AND_DIFFUSE;
	c->color_material_enabled = 0;

	/* textures */
	/*glInitTextures(c);*/
	glInitTextures(); //Bug Fix!

	/* blending */
	c->zb->enable_blend = 0;
	c->zb->sfactor = GL_ONE;
	c->zb->dfactor = GL_ZERO;
	c->zb->blendeq = GL_FUNC_ADD;

	/* default state */
	c->current_color.X = 1.0;
	c->current_color.Y = 1.0;
	c->current_color.Z = 1.0;
	c->current_color.W = 0.0;

	c->current_normal.X = 1.0;
	c->current_normal.Y = 0.0;
	c->current_normal.Z = 0.0;
	c->current_normal.W = 0.0;

	c->current_edge_flag = 1;

	c->current_tex_coord.X = 0;
	c->current_tex_coord.Y = 0;
	c->current_tex_coord.Z = 0;
	c->current_tex_coord.W = 1;

	c->polygon_mode_front = GL_FILL;
	c->polygon_mode_back = GL_FILL;

	c->current_front_face = 0; /* 0 = GL_CCW  1 = GL_CW */
	c->current_cull_face = GL_BACK;
	c->current_shade_model = GL_SMOOTH;
	c->cull_face_enabled = 0;

#if TGL_FEATURE_POLYGON_STIPPLE == 1
	c->zb->dostipple = 0;
	for (GLint i = 0; i < 128; i++)
		c->zb->stipplepattern[i] = 0xFF;
#endif
	/* clear */
	c->clear_color.v[0] = 0;
	c->clear_color.v[1] = 0;
	c->clear_color.v[2] = 0;
	c->clear_color.v[3] = 0;
	c->clear_depth = 0;

	/* selection */
#if TGL_FEATURE_ALT_RENDERMODES == 1
	c->render_mode = GL_RENDER;
	c->select_buffer = NULL;
	c->name_stack_size = 0;

	/* feedback */
	c->feedback_buffer = NULL;
	c->feedback_ptr = NULL;
	c->feedback_size = 0;
	c->feedback_hits = 0;
	c->feedback_overflow = 0;
#endif
	/* matrix */
	c->matrix_mode = 0;

	c->matrix_stack_depth_max[0] = MAX_MODELVIEW_STACK_DEPTH;
	c->matrix_stack_depth_max[1] = MAX_PROJECTION_STACK_DEPTH;
	c->matrix_stack_depth_max[2] = MAX_TEXTURE_STACK_DEPTH;

	for (i = 0; i < 3; i++) {
		c->matrix_stack[i] = gl_zalloc(c->matrix_stack_depth_max[i] * sizeof(M4));
		if (!(c->matrix_stack[i]))
			gl_fatal_error("TINYGL_CANNOT_INIT_OOM");
		c->matrix_stack_ptr[i] = c->matrix_stack[i];
	}

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	c->matrix_model_projection_updated = 1;

	/* opengl 1.1 arrays */
	c->client_states = 0;

	/* opengl 1.1 polygon offset */
	c->offset_states = 0;

	/* clear the resize callback function pointer */
	c->gl_resize_viewport = NULL;

	/* specular buffer */
#if TGL_FEATURE_SPECULAR_BUFFERS == 1
	c->specbuf_first = NULL;
	c->specbuf_used_counter = 0;
	c->specbuf_num_buffers = 0;
#endif
	c->zEnableSpecular = 0;
	/* depth test */
	c->zb->depth_test = 0;
	c->zb->depth_write = 1;
	c->zb->pointsize = 1;

	/* raster position */
	c->rasterpos.X = 0;
	c->rasterpos.Y = 0;
	c->rasterpos.Z = 0;
	c->rasterpos.W = 1;

	c->rastervertex.pc.X = 0;
	c->rastervertex.pc.Y = 0;
	c->rastervertex.pc.Z = 0;
	c->rastervertex.pc.W = 1;
	c->rasterposvalid = 0;
	c->pzoomx = 1;
	c->pzoomy = 1;
}

void glClose(void) {

	GLuint i;
	GLContext* c = gl_get_context();
	for (i = 0; i < 3; i++) {
		gl_free(c->matrix_stack[i]);
	}
	i = 0;
#if TGL_FEATURE_SPECULAR_BUFFERS == 1
	{
		GLSpecBuf *b, *n = NULL;
		for (b = c->specbuf_first; b != NULL; b = n) {
			n = b->next;
			gl_free(b);
			i++;
		}
	}
#endif
	endSharedState(c);
	gl_ctx = empty_gl_ctx;
}
