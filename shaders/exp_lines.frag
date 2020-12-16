#version 330 core

out vec4 fragColor;
in vec3 line_color;

void main() {
	fragColor = vec4(line_color, 1.0);	
}
