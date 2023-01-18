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

vec4 vram_read(uint x, uint y)
{
    return texelFetch(vram_texture, ivec2(int(x), int(y)), 0);
}

// Thanks Avocado (https://github.com/JaCzekanski/Avocado)!

uint unpack_texel(vec4 texel)
{
    uint i0 = uint(texel.a);

    texel *= ivec4(0x1f);
    uint i1 = uint(texel.b + 0.5);
    uint i2 = uint(texel.g + 0.5);
    uint i3 = uint(texel.r + 0.5);

    return (i0 << 15) | (i1 << 10) | (i2 << 5) | i3;
}



vec4 sample_texel(vec2 tc)
{
    vec4 pixel = vec4(1.0, 0.0, 0.0, 1.0);
    float stride_divisor = 16.0;

    if (frag_texture_depth == 0u)
    {
        stride_divisor /= 4.0;
        int tc_x = int(frag_texture_page.x) + int(tc.x / stride_divisor);
        int tc_y = int(frag_texture_page.y) + int(tc.y);

        // Fetch texel from VRAM. Each 4-bits is an index into the CLUT
        uint texel = unpack_texel(vram_read(uint(tc_x), uint(tc_y)));

        // We now fetch each pixel (located in the clut) in the texel in
        // 4bit chunks, based on the current texture co-ordinate's x value.
        // Therefore, for each fragment on screen, we read out 4-pixels worth
        // of data from VRAM (or so I think..) based on this texcoord x-value.
        uint clut_index = ((texel >> ((uint(tc.x) % 4u) * 4u)) & 0xfu);
        pixel = vram_read(frag_clut.x + clut_index, uint(frag_clut.y));
    }
    else if (frag_texture_depth == 1u)
    {
        int tc_x = int(frag_texture_page.x) + int(tc.x / 2);
        int tc_y = int(frag_texture_page.y) + int(tc.y);
        
        if (frag_blend_mode == RAW_TEXTURE)
        {
            uint texel = unpack_texel(texelFetch(vram_texture, ivec2(ivec2(tc.x / 2, tc.y) + ivec2(frag_texture_page)), 0));

            uint clut_index = ((texel >> ((uint(tc.x) % 2u) * 8u)) & 0xFFu);
            pixel = vram_read(frag_clut.x + clut_index, uint(frag_clut.y));
        }
    }
    else if (frag_texture_depth == 2u)
    {
        stride_divisor /= 16.0;
        int tc_x = int(frag_texture_page.x) + int(tc.x / stride_divisor);
        int tc_y = int(frag_texture_page.y) + int(tc.y);
        
        if (frag_blend_mode == RAW_TEXTURE)
        {
            pixel = vram_read(uint(tc_x), uint(tc_y));
        }
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