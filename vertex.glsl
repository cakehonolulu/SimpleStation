#version 330 core

in ivec2 vertex_position;
in uvec3 vertex_color;

out vec3 color;

void main() {
  // NOTE: mapping VRAM to OpenGL, (0;1023, 0;511) -> (-1;1, -1;1)

  float xpos = (float(vertex_position.x) / 640 * 2) - 1.0;

  // NOTE: VRAM top-left based, OpenGL bottom-left
  float ypos = (float(vertex_position.y) / 480 * (-2) + 1);

  gl_Position.xyzw = vec4(xpos, ypos, 0.0, 1.0);

  color = vec3(float(vertex_color.r) / 255,
	       float(vertex_color.g) / 255,
	       float(vertex_color.b) / 255);
}
