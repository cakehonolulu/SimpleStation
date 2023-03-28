#version 330 core

in vec3 color;
flat in uvec2 frag_texture_page;
in vec2 frag_texture_coord;
flat in uvec2 frag_clut;
flat in uint frag_texture_depth;
flat in uint frag_blend_mode;
flat in uint frag_texture_draw;

out vec4 frag_color;

uniform sampler2D vram_texture;

#define NO_TEXTURE      0U
#define RAW_TEXTURE     1U
#define TEXTURE_BLEND   2U

vec4 vramRead(uint x, uint y)
{
    return texelFetch(vram_texture, ivec2(x, y), 0);
}

// Thanks Avocado (https://github.com/JaCzekanski/Avocado)!

uint internalToPsxColor(vec4 texel)
{
    uint i0 = uint(texel.a);

    texel *= ivec4(0x1f);
    uint i1 = uint(texel.b + 0.5);
    uint i2 = uint(texel.g + 0.5);
    uint i3 = uint(texel.r + 0.5);

    return (i0 << 15) | (i1 << 10) | (i2 << 5) | i3;
}


vec4 clut4bit(uvec2 clut) {
    int texX = int(frag_texture_page.x) + int(frag_texture_coord.x / 4.0);
    int texY = int(frag_texture_page.y) + int(frag_texture_coord.y);

    vec4 vram = vramRead(uint(texX), uint(texY));

    uint index = internalToPsxColor(vram);
    uint which = (index >> ((uint(frag_texture_coord.x) & 3u) * 4u)) & 0xfu;

    return vramRead(uint(clut.x) + uint(which), uint(clut.y));
}

vec4 clut8bit(uvec2 clut) {
    int texX = int(frag_texture_page.x) + int(frag_texture_coord.x / 2.0);
    int texY = int(frag_texture_page.y) + int(frag_texture_coord.y);

    uint index = internalToPsxColor(vramRead(uint(texX), uint(texY)));
    uint which = (index >> ((uint(frag_texture_coord.x) & 1u) * 8u)) & 0xffu;

    return vramRead(uint(clut.x) + which, uint(clut.y));
}

vec4 read16bit() { return vramRead(uint(frag_texture_page.x) + uint(frag_texture_coord.x), uint(frag_texture_page.y) + uint(frag_texture_coord.y)); }

vec2 calculateTexel(vec2 texcoord) {
    uvec2 texel = uvec2(uint(texcoord.x) % 256u, uint(texcoord.y) % 256u);

    uvec2 mask = uvec2((frag_blend_mode) & 0x1fu, (frag_blend_mode >> 5) & 0x1fu);
    uvec2 offset = uvec2((frag_blend_mode >> 10) & 0x1fu, (frag_blend_mode >> 15) & 0x1fu);

    texel.x = (texel.x & ~(mask.x * 8u)) | ((offset.x & mask.x) * 8u);
    texel.y = (texel.y & ~(mask.y * 8u)) | ((offset.y & mask.y) * 8u);

    return vec2(texel);
}


vec4 sample_texel(vec2 tc)
{
    vec4 pixel = vec4(1.0, 0.0, 0.0, 1.0);
    
    if (frag_texture_depth == 0u) {
        pixel = clut4bit(frag_clut);
    } else if (frag_texture_depth == 1u) {
        pixel = clut8bit(frag_clut);
    } else if (frag_texture_depth == 2u) {
        pixel = read16bit();
    }

    if (pixel == vec4(0))
        discard;

    return pixel;
}



void main() {
	
	// Texture drawing
	if (frag_texture_draw == 1U)
	{
        frag_color = sample_texel(frag_texture_coord);
    }
	else
	{
		// Triangle with Goraud Shading
		frag_color = vec4(color, 1.0);
	}

}