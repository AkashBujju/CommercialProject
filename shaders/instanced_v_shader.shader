#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

out vec3 fragPos;
out vec3 Normal;
out Material material;

uniform mat4 models[10];
uniform Material materials[10];
uniform mat4 view;
uniform mat4 projection;

void main() {
	material = materials[gl_InstanceID];
	mat4 model = models[gl_InstanceID];

	fragPos = vec3(model * vec4(aPos, 1.0));
	Normal = mat3(transpose(inverse(model))) * aNormal;
	gl_Position = projection * view * vec4(fragPos, 1.0f);
}
