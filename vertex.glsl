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
flat out int draw_vram;

uniform ivec2 offset;


uniform int disp_vram_frame;

void main() {
  // NOTE: mapping VRAM to OpenGL, (0;1023, 0;511) -> (-1;1, -1;1)

  ivec2 position = vertex_position + offset;

  float xpos = (float(position.x + 0.5) / 640.0 * 2.0) - 1.0;

  // NOTE: VRAM top-left based, OpenGL bottom-left
  float ypos = (float(position.y - 0.5) / 480.0 * (-2.0) + 1.0);

  if (disp_vram_frame == 1)
  {
    const vec4 positions[4] = vec4[](
      vec4(-1.0, 1.0, 1.0, 1.0),    // Top-left
      vec4(1.0, 1.0, 1.0, 1.0),     // Top-right
      vec4(-1.0, -1.0, 1.0, 1.0),   // Bottom-left
      vec4(1.0, -1.0, 1.0, 1.0)     // Bottom-right
    );
 
    const vec2 texcoords[4] = vec2[](     //Inverted in Y because PS1 Y coords are inverted
      vec2(0.0, 0.0),   // Top-left
      vec2(1.0, 0.0),   // Top-right
      vec2(0.0, 1.0),   // Bottom-left
      vec2(1.0, 1.0)    // Bottom-right
    );
    
    gl_Position = positions[gl_VertexID];
    frag_texture_coord = texcoords[gl_VertexID];
    draw_vram = 1;
    return;
  }
  else
  {
    gl_Position = vec4(xpos, ypos, 0.0, 1.0);
    draw_vram = 0;
  }
  

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
