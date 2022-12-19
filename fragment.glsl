#version 330 core

in vec3 color;
flat in uvec2 frag_texture_page;
flat in vec2 frag_texture_coord;
flat in uvec2 frag_clut;
flat in uint frag_texture_depth;
flat in uint frag_blend_mode;
flat in uint frag_texture_draw;

out vec4 frag_color;

void main() {
	
	// No texture drawing
	if (frag_texture_draw == 4U)
	{
		frag_color = vec4(color, 1.0);
	}
	else
	{
		// Draw texture, hardcoded to 4bpp mode for now
		frag_color = vec4(float(color.r) * 155,
	       float(color.g) * 155,
	       float(color.b) * 155,
		   1.0);
	}

}