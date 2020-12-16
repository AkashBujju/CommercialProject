#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 line_color;

uniform mat4 view;
uniform mat4 projection;

void main() {
	mat4 model;
	model[0] = vec4(1, 0, 0, 0);
	model[1] = vec4(0, 1, 0, 0);
	model[2] = vec4(0, 0, 1, 0);
	model[3] = vec4(0, 0, 0, 1);
	
	line_color = aColor;
	gl_Position = projection * view * model * vec4(aPos, 1.0);
}
