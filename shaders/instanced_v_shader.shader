#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 fragPos;
out vec3 Normal;
out vec3 objectColor;

uniform mat4 models[10];
uniform vec3 objectColors[10];
uniform mat4 view;
uniform mat4 projection;

void main() {
	objectColor = objectColors[gl_InstanceID];

	mat4 model = models[gl_InstanceID];
	fragPos = vec3(model * vec4(aPos, 1.0));
	Normal = mat3(transpose(inverse(model))) * aNormal;
	gl_Position = projection * view * vec4(fragPos, 1.0f);
}
