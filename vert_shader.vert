#version 450 core

layout(location=0) in vec2 vertex_position;
layout(location=1) in vec3 vertex_color;

layout(location=0) out vec3 color;

vec2 positions[3]=vec2[](
	vec2(0.0,-0.5),
	vec2(0.5,0.5),
	vec2(-0.5,0.5)
);

vec3 colors[3]=vec3[](
	vec3(1,0.0,0.0),
	vec3(0.0,1,0.0),
	vec3(0.0,0.0,1)
);

void main() {
  // NOTE: mapping VRAM to OpenGL, (0;1023, 0;511) -> (-1;1, -1;1)

  float xpos = (float(vertex_position.x) / 320) - 1.0;
  float ypos = -1.0 + (float(vertex_position.y) / 240);


  gl_Position = vec4(xpos, ypos, 0.0, 1.0);

  color = vec3(float(vertex_color.r) / 255.0,
	       float(vertex_color.g) / 255.0,
	       float(vertex_color.b) / 255.0);
}
