#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec4 aModel[4];
layout (location = 6) in vec3 aAmbient;
layout (location = 7) in vec3 aDiffuse;
layout (location = 8) in vec3 aSpecular;
layout (location = 9) in float aShininess;

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

out vec3 fragPos;
out vec3 Normal;
out Material material;

uniform mat4 view;
uniform mat4 projection;

void main() {
	material.ambient = aAmbient;
	material.diffuse = aDiffuse;
	material.specular = aSpecular;
	material.shininess = aShininess;

	mat4 md;
	md[0] = aModel[0];
	md[1] = aModel[1];
	md[2] = aModel[2];
	md[3] = aModel[3];

	fragPos = vec3(md * vec4(aPos, 1.0));
	Normal = mat3(transpose(inverse(md))) * aNormal;
	gl_Position = projection * view * vec4(fragPos, 1.0f);
}
