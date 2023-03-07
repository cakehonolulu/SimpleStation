#version 450 core

layout(location=0) in vec3 color;

layout(location=0) out vec4 frag_color;

void main() {
	// Triangle with Goraud Shading
	frag_color = vec4(color, 1.0);
}
