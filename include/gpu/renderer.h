#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <simplestation.h>

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>

#include <SDL.h>

#define VERTEX_BUFFER_LEN 64 * 1024

typedef struct {
	GLshort x;
	GLshort y;
} RendererPosition;

// Color
typedef struct {
	GLubyte r;
	GLubyte g;
	GLubyte b;
} RendererColor;


RendererPosition pos_from_gp0(uint32_t val);
RendererColor color_from_gp0(uint32_t val);
void display();

uint8_t m_renderer_init(m_simplestation_state *m_simplestation);
uint8_t m_renderer_buffers_init(m_simplestation_state *m_simplestation);
GLuint renderer_LoadShader(char *path, GLenum type);
int put_triangle(const RendererPosition positions[3], const RendererColor colors[3]);
int put_quad(const RendererPosition positions[4], const RendererColor colors[4]);
