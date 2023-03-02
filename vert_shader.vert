#version 450 core

layout(location=0) in vec2 vertex_position;
layout(location=1) in vec3 vertex_color;

layout(location=0) out vec3 color;

void main() {
  // NOTE: mapping VRAM to OpenGL, (0;1023, 0;511) -> (-1;1, -1;1)

  float x = float(vertex_position.x);
  float y = float(vertex_position.y);

  float xx = (x) / 512.0;
  float yy = (y) / 256;
  // Normalize to [-1, 1]
  xx -= 1.0;
  yy -= 1.0;


  gl_Position = vec4(xx, yy, 0.0, 1.0);

  color = vec3(float(vertex_color.r) / 255.0,
	       float(vertex_color.g) / 255.0,
	       float(vertex_color.b) / 255.0);
}
