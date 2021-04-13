#include "msghandling.h"
#include "zgl.h"

/*
static char* op_table_str[] = {
#define ADD_OP(a, b, c) "gl" #a " " #c,

#include "opinfo.h"
};
*/

void (*op_table_func[])(GLParam*) = {
#define ADD_OP(a, b, c) glop##a,

#include "opinfo.h"
};

GLint op_table_size[] = {
#define ADD_OP(a, b, c) b + 1,

#include "opinfo.h"
};

static GLList* find_list(GLuint list) { return gl_get_context()->shared_state.lists[list]; }

static void delete_list(GLint list) {
	GLContext* c = gl_get_context();
	GLParamBuffer *pb, *pb1;
	GLList* l;

	l = find_list(list);
	if (l == NULL) { 
		return;
	}
	

	/* free param buffer */
	pb = l->first_op_buffer;
	while (pb != NULL) {
		pb1 = pb->next;
		gl_free(pb);
		pb = pb1;
	}

	gl_free(l);
	c->shared_state.lists[list] = NULL;
}
void glDeleteLists(GLuint list, GLuint range) {
	GLuint i;
#include "error_check_no_context.h"
	for (i = 0; i < list + range; i++)
		glDeleteList(list + i);
}
void glDeleteList(GLuint list) {
#include "error_check_no_context.h"
	delete_list(list);
}

static GLList* alloc_list(GLint list) {
	GLList* l;
	GLParamBuffer* ob;
	GLContext* c = gl_get_context();
#define RETVAL NULL
#include "error_check.h"
	l = gl_zalloc(sizeof(GLList));
	ob = gl_zalloc(sizeof(GLParamBuffer));

#if TGL_FEATURE_ERROR_CHECK
	if (!l || !ob)
#define ERROR_FLAG GL_OUT_OF_MEMORY
#define RETVAL NULL
#include "error_check.h"

#else
	/* 
	if(!l || !ob) gl_fatal_error("GL_OUT_OF_MEMORY");
	 This will crash a few lines down, so, let it!
	 */
#endif
		ob->next = NULL;
	l->first_op_buffer = ob;

	ob->ops[0].op = OP_EndList;

	c->shared_state.lists[list] = l;
	return l;
}
/*
void gl_print_op(FILE* f, GLParam* p) {
	GLint op;
	char* s;

	op = p[0].op;
	p++;
	s = op_table_str[op];
	while (*s != 0) {
		if (*s == '%') {
			s++;
			switch (*s++) {
			case 'f':
				fpr_ntf(f, "%g", p[0].f);
				break;
			default:
				fpr_ntf(f, "%d", p[0].i);
				break;
			}
			p++;
		} else {
			fputc(*s, f);
			s++;
		}
	}
	tgl_warning(f, "\n");
}
*/
void glListBase(GLint n) {
	GLContext* c = gl_get_context();
#include "error_check.h"
	c->listbase = n;
}
void glCallLists(GLsizei n, GLenum type, const GLuint* lists) {
	GLint i;
	GLContext* c = gl_get_context();
	#include "error_check.h"
	#if TGL_FEATURE_ERROR_CHECK == 1
		if(type != GL_UNSIGNED_INT &&
			type != GL_INT)
	#define ERROR_FLAG GL_INVALID_ENUM
	#include "error_check.h"
	#endif
	for (i = 0; i < n; i++)
		glCallList(c->listbase + lists[i]);
}
void gl_compile_op(GLParam* p) {
	GLContext* c = gl_get_context();
	GLint op, op_size;
	GLParamBuffer *ob, *ob1;
	GLint index, i;
#include "error_check.h"
	op = p[0].op;
	op_size = op_table_size[op];
	index = c->current_op_buffer_index;
	ob = c->current_op_buffer;

	/* we should be able to add a NextBuffer opcode */
	if ((index + op_size) > (OP_BUFFER_MAX_SIZE - 2)) {

		ob1 = gl_zalloc(sizeof(GLParamBuffer));

#if TGL_FEATURE_ERROR_CHECK == 1
		if (!ob1)
#define ERROR_FLAG GL_OUT_OF_MEMORY
#include "error_check.h"
#else
		
		
#endif
			ob1->next = NULL;

		ob->next = ob1;
		ob->ops[index].op = OP_NextBuffer;
		ob->ops[index + 1].p = (void*)ob1;

		c->current_op_buffer = ob1;
		ob = ob1;
		index = 0;
	}

	for (i = 0; i < op_size; i++) {
		ob->ops[index] = p[i];
		index++;
	}
	c->current_op_buffer_index = index;
}
/* this opcode is never called directly */
void glopEndList(GLParam* p) { exit(1); }

/* this opcode is never called directly */
void glopNextBuffer(GLParam* p) { exit(1); }

void glopCallList(GLParam* p) {
	
	GLList* l;
	GLint list;
#include "error_check_no_context.h"
	list = p[1].ui;
	l = find_list(list);

#if TGL_FEATURE_ERROR_CHECK == 1
	if (l == NULL) {
		gl_fatal_error("Bad list op, not defined");
	}
#else
	
#endif
	p = l->first_op_buffer->ops;

	while (1) {
		GLint op;
#include "error_check.h"
		op = p[0].op;
		if (op == OP_EndList)
			break;
		if (op == OP_NextBuffer) {
			p = (GLParam*)p[1].p;
		} else {
			op_table_func[op](p);
			p += op_table_size[op];
		}
	}
}

void glNewList(GLuint list, GLint mode) {
	GLList* l;
	GLContext* c = gl_get_context();
#include "error_check.h"

#if TGL_FEATURE_ERROR_CHECK == 1

	if (!(mode == GL_COMPILE || mode == GL_COMPILE_AND_EXECUTE))
#define ERROR_FLAG GL_INVALID_ENUM
#include "error_check.h"

		if (!(c->compile_flag == 0))
#define ERROR_FLAG GL_INVALID_OPERATION
#include "error_check.h"

#else
	
	
#endif
			l = find_list(list);
	if (l != NULL)
		delete_list(list);
	l = alloc_list(list);
#include "error_check.h"
#if TGL_FEATURE_ERROR_CHECK == 1
	if (l == NULL)
#define ERROR_FLAG GL_OUT_OF_MEMORY
#include "error_check.h"
#else
	
	if (l == NULL)
		gl_fatal_error("Could not find or allocate list.");
#endif
		c->current_op_buffer = l->first_op_buffer;
	c->current_op_buffer_index = 0;

	c->compile_flag = 1;
	c->exec_flag = (mode == GL_COMPILE_AND_EXECUTE);
}

void glEndList(void) {
	GLContext* c = gl_get_context();
	GLParam p[1];
#include "error_check.h"
#if TGL_FEATURE_ERROR_CHECK == 1
	if (c->compile_flag != 1)
#define ERROR_FLAG GL_INVALID_OPERATION
#include "error_check.h"
#else
	if (c->compile_flag != 1)
		return;
#endif
		/* end of list */
		p[0].op = OP_EndList;
	gl_compile_op(p);

	c->compile_flag = 0;
	c->exec_flag = 1;
}

GLint glIsList(GLuint list) {
	
	GLList* l;
	l = find_list(list);
	return (l != NULL);
}

GLuint glGenLists(GLint range) {
	GLint count, i, list;
	GLList** lists;
	GLContext* c = gl_get_context();
#define RETVAL 0
#include "error_check.h"
	lists = c->shared_state.lists;
	count = 0;
	for (i = 0; i < MAX_DISPLAY_LISTS; i++) {
		if (lists[i] == NULL) {
			count++;
			if (count == range) {
				list = i - range + 1;
				for (i = 0; i < range; i++) {
					alloc_list(list + i);
				}
				return list;
			}
		} else {
			count = 0;
		}
	}
	return 0;
}
