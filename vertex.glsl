#version 330 core

in ivec2 vertex_position;
in uvec3 vertex_color;
in uvec2 texture_page;
in uvec2 texture_coord;
in uvec2 clut;
in uint texture_depth;
in uint texture_blend_mode;

out vec3 color;
flat out uvec2 frag_texture_page;
out vec2 frag_texture_coord;
flat out uvec2 frag_clut;
flat out uint frag_texture_depth;
flat out uint frag_blend_mode;

void main() {
  // NOTE: mapping VRAM to OpenGL, (0;1023, 0;511) -> (-1;1, -1;1)

  float xpos = (float(vertex_position.x) / 640 * 2) - 1.0;

  // NOTE: VRAM top-left based, OpenGL bottom-left
  float ypos = (float(vertex_position.y) / 480 * (-2) + 1);

  gl_Position.xyzw = vec4(xpos, ypos, 0.0, 1.0);

  color = vec3(float(vertex_color.r) / 255,
	       float(vertex_color.g) / 255,
	       float(vertex_color.b) / 255);

  frag_texture_page = texture_page;
  frag_texture_coord = vec2(texture_coord);
  frag_clut = clut;
  frag_texture_depth = texture_depth;
  frag_blend_mode = texture_blend_mode;
}
