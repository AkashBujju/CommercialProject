#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec4 aModel[4];
layout (location = 6) in vec4 aColor;

out vec3 fragPos;
out vec4 objectColor;
out vec3 Normal;

uniform mat4 view;
uniform mat4 projection;

void main() {
	mat4 md;
	md[0] = aModel[0];
	md[1] = aModel[1];
	md[2] = aModel[2];
	md[3] = aModel[3];

	objectColor = aColor;
	Normal = mat3(transpose(inverse(md))) * aNormal;
	fragPos = vec3(md * vec4(aPos, 1.0));
	gl_Position = projection * view * vec4(fragPos, 1.0f);
}
