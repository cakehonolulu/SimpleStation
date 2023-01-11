#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <simplestation.h>


#include <GL/glew.h>

#include <SDL2/SDL.h>

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
} Vertex;
#pragma pack(pop)

Position pos_from_gp0(uint32_t val);
Colour color_from_gp0(uint32_t val);
Colour color(GLubyte r, GLubyte g, GLubyte b);
void display(m_simplestation_state *m_simplestation);
void m_window_changetitle(char *buffer);
Position pos_from_gp0(uint32_t value);
Colour col_from_gp0(uint32_t value);
TexPage texpage_from_gp0(uint32_t value);
TexCoord texcoord_from_gp0(uint32_t value);
ClutAttr clutattr_from_gp0(uint32_t value);
TextureColourDepth tcd_from_gp0(uint32_t value);
TextureColourDepth tcd_from_val(textureColourDepthValue value);

void m_texture_upload(m_simplestation_state *m_simplestation);

void renderer_LoadImage(m_simplestation_state *m_simplestation);

void draw(m_simplestation_state *m_simplestation, bool clear_colour);

void m_renderer_setup_onscreen(m_simplestation_state *m_simplestation);
void m_renderer_setup_offscreen(m_simplestation_state *m_simplestation);
void m_sync_vram(m_simplestation_state *m_simplestation);
uint8_t m_renderer_init(m_simplestation_state *m_simplestation);
void m_renderer_buffers_init();
GLuint renderer_LoadShader(char *path, GLenum type);
int put_triangle(Vertex v1, Vertex v2, Vertex v3);
int put_quad(Vertex v1, Vertex v2, Vertex v3, Vertex v4);
