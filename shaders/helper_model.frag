#version 330 core
out vec4 fragColor;

struct DirLight {
	vec3 position;
};

in vec3 Normal;
in vec3 fragPos;
in vec3 objectColor;

uniform vec3 viewPos;
uniform vec3 lightColor;
uniform DirLight dir_lights[10];
uniform int num_dir_lights;

vec3 calculate_dir_light(DirLight dir_light, vec3 normal, vec3 viewDir);

void main() {
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - fragPos);
	vec3 result = vec3(0, 0, 0);

	for(int i = 0; i < num_dir_lights; ++i)
		result += calculate_dir_light(dir_lights[i], norm, viewDir);

	fragColor = vec4(result, 1.0);
}

vec3 calculate_dir_light(DirLight dir_light, vec3 normal, vec3 viewDir) {
	float ambient_strength = 0.1;
	vec3 ambient = ambient_strength * lightColor;

	vec3 lightDir = normalize(dir_light.position - fragPos);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;

	float specular_strength = 0.5;
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = specular_strength * spec * lightColor;

	vec3 result = (ambient + diffuse + specular) * objectColor;
	return result;
}
