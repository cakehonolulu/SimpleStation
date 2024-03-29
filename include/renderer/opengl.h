#pragma once

#include <GL/glew.h>
#if defined(__APPLE__)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <simplestation.h>

#define VERTEX_BUFFER_LEN 64 * 1024

typedef enum
{
	texDepth4Bit = 0,
	texDepth8Bit = 1,
	texDepth15Bit = 2
} textureColourDepthValue;

typedef enum
{
	NoTexture = 0,
	RawTexture = 1,
	BlendTexture = 2
} BlendMode;


#pragma pack(push, 1)
typedef struct
{
	GLshort x;
	GLshort y;
} Position;

typedef struct
{
	GLubyte r;
	GLubyte g;
	GLubyte b;
} Colour;

typedef struct
{
	GLushort xBase;
	GLushort yBase;
} TexPage;

typedef struct
{
	GLubyte x;
	GLubyte y;
} TexCoord;

typedef struct
{
	GLushort x;
	GLushort y;
} ClutAttr;

typedef struct
{
	GLubyte depth;
} TextureColourDepth;

typedef struct
{
	Position position;
	Colour colour;
	TexPage texPage;
	TexCoord texCoord;
	ClutAttr clut;
	TextureColourDepth texDepth;
	GLubyte blendMode;
	GLubyte drawTexture;
} OpenGL_Vertex;
#pragma pack(pop)


typedef struct
{
	GLshort width;
	GLshort height;
} RectWidthHeight;

typedef struct
{
	Position position;
	Colour colour;
	TexPage texPage;
	TexCoord texCoord;
	ClutAttr clut;
	TextureColourDepth texDepth;
	GLubyte blendMode;
	GLubyte drawTexture;
	RectWidthHeight widthHeight;
} Rectangle;

Position pos_from_gp0(uint32_t val);
Colour color_from_gp0(uint32_t val);
Colour color(GLubyte r, GLubyte g, GLubyte b);

void m_window_changetitle(char *buffer);
Position pos_from_gp0(uint32_t value);
Colour col_from_gp0(uint32_t value);
TexPage texpage_from_gp0(uint32_t value);
TexCoord texcoord_from_gp0(uint32_t value);
ClutAttr clutattr_from_gp0(uint32_t value);
TextureColourDepth tcd_from_gp0(uint32_t value);
TextureColourDepth tcd_from_val(textureColourDepthValue value);
RectWidthHeight rwh_from_gp0(uint32_t value);


void display(m_simplestation_state *m_simplestation);

void m_texture_upload(m_simplestation_state *m_simplestation);

void renderer_LoadImage(m_simplestation_state *m_simplestation);

void draw(m_simplestation_state *m_simplestation, bool clear_colour, bool part, bool isline);

void m_renderer_update_display_area(m_simplestation_state *m_simplestation);
void m_renderer_setup_onscreen();
void m_renderer_setup_offscreen(m_simplestation_state *m_simplestation);
void m_sync_vram(m_simplestation_state *m_simplestation);
uint8_t init_opengl_renderer(m_simplestation_state *m_simplestation);
void m_renderer_buffers_init();
GLuint renderer_LoadShader(char *path, GLenum type);
int put_line(OpenGL_Vertex v1, OpenGL_Vertex v2, m_simplestation_state *m_simplestation);
int put_triangle(OpenGL_Vertex v1, OpenGL_Vertex v2, OpenGL_Vertex v3, m_simplestation_state *m_simplestation);
int put_quad(OpenGL_Vertex v1, OpenGL_Vertex v2, OpenGL_Vertex v3, OpenGL_Vertex v4, m_simplestation_state *m_simplestation);
int put_rect(Rectangle r0, m_simplestation_state *m_simplestation);

