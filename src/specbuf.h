#ifndef _tgl_specbuf_h_
#define _tgl_specbuf_h_

/* Max # of specular light pow buffers */
#define MAX_SPECULAR_BUFFERS 8
/* # of entries in specular buffer */
#define SPECULAR_BUFFER_SIZE 1024
/* specular buffer granularity */
#define SPECULAR_BUFFER_RESOLUTION 1024

typedef struct GLSpecBuf {
  GLint shininess_i;
  GLint last_used;
  GLfloat buf[SPECULAR_BUFFER_SIZE+1];
  struct GLSpecBuf *next;
} GLSpecBuf;

GLSpecBuf *specbuf_get_buffer(GLContext *c, const GLint shininess_i, 
                              const GLfloat shininess);
void specbuf_cleanup(GLContext *c); /* free all memory used */

#endif /* _tgl_specbuf_h_ */