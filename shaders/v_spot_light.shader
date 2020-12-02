#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aModel[4];

uniform mat4 view;
uniform mat4 projection;

void main() {
	mat4 md;
	md[0] = aModel[0];
	md[1] = aModel[1];
	md[2] = aModel[2];
	md[3] = aModel[3];

	vec3 fragPos = vec3(md * vec4(aPos, 1.0));
	gl_Position = projection * view * vec4(fragPos, 1.0);
}
