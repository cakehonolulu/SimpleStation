#version 330 core

layout (location = 0) in ivec2 vertex_position;
layout (location = 1) in uvec3 vertex_color;
layout (location = 2) in uvec2 texture_page;
layout (location = 3) in uvec2 texture_coord;
layout (location = 4) in uvec2 clut;
layout (location = 5) in uint texture_depth;
layout (location = 6) in uint texture_blend_mode;
layout (location = 7) in uint texture_draw;

out vec3 color;
flat out uvec2 frag_texture_page;
out vec2 frag_texture_coord;
flat out uvec2 frag_clut;
flat out uint frag_texture_depth;
flat out uint frag_blend_mode;
flat out uint frag_texture_draw;

uniform ivec2 offset;

void main() {
  // NOTE: mapping VRAM to OpenGL, (0;1023, 0;511) -> (-1;1, -1;1)

  ivec2 position = vertex_position + offset;

  float x = float(position.x);
  float y = float(position.y);

  float xx = (x + offset.x) / 512.0;
  float yy = (y + offset.y) / 256;
  // Normalize to [-1, 1]
  xx -= 1.0;
  yy -= 1.0;


  gl_Position = vec4(xx, yy, 0.0, 1.0);

  color = vec3(float(vertex_color.r) / 255.0,
	       float(vertex_color.g) / 255.0,
	       float(vertex_color.b) / 255.0);
  
  frag_texture_coord = vec2(texture_coord.x, texture_coord.y);
  frag_texture_page = texture_page;
  frag_clut = clut;
  frag_texture_depth = texture_depth;
  frag_blend_mode = texture_blend_mode;
  frag_texture_draw = texture_draw;
}
