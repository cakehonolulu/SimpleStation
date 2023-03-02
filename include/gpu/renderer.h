#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <simplestation.h>

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
	int16_t x;
	int16_t y;
} Position;

typedef struct
{
	uint8_t r;
	uint8_t g;
	uint8_t b;
} Colour;

typedef struct
{
	uint16_t xBase;
	uint16_t yBase;
} TexPage;

typedef struct
{
	uint8_t x;
	uint8_t y;
} TexCoord;

typedef struct
{
	uint16_t x;
	uint16_t y;
} ClutAttr;

typedef struct
{
	uint8_t depth;
} TextureColourDepth;

typedef struct
{
	Position position;
	Colour colour;
	TexPage texPage;
	TexCoord texCoord;
	ClutAttr clut;
	TextureColourDepth texDepth;
	uint8_t blendMode;
	uint8_t drawTexture;
} Vertex2;
#pragma pack(pop)

void pos_from_gp0(uint32_t val, vec2 *vec);
void col_from_gp0(uint32_t value, vec3 *vec);
Colour color(uint8_t r, uint8_t g, uint8_t b);
void display(m_simplestation_state *m_simplestation);
void m_window_changetitle(char *buffer);
TexPage texpage_from_gp0(uint32_t value);
TexCoord texcoord_from_gp0(uint32_t value);
ClutAttr clutattr_from_gp0(uint32_t value);
TextureColourDepth tcd_from_gp0(uint32_t value);
TextureColourDepth tcd_from_val(textureColourDepthValue value);

void m_texture_upload(m_simplestation_state *m_simplestation);

void renderer_LoadImage(m_simplestation_state *m_simplestation);

void draw(m_simplestation_state *m_simplestation, bool clear_colour);

void m_renderer_setup_onscreen();
void m_renderer_setup_offscreen();
void m_sync_vram(m_simplestation_state *m_simplestation);
uint8_t m_renderer_init(m_simplestation_state *m_simplestation);
void m_renderer_buffers_init();
int put_triangle(Vertex v1, Vertex v2, Vertex v3, m_simplestation_state *m_simplestation);
int put_quad(Vertex v1, Vertex v2, Vertex v3, Vertex v4);
