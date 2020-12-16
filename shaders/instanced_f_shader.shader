#version 330 core
out vec4 fragColor;

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

struct DirLight {
	vec3 position;
};

struct SpotLight {
	vec3 position;  
   vec3 direction;
};

in vec3 Normal;
in vec3 fragPos;
in Material material;
in float alpha_value;

uniform vec3 viewPos;
uniform vec3 light_ambient;
uniform vec3 light_diffuse;
uniform vec3 light_specular;
uniform float light_constant;
uniform float light_linear;
uniform float light_quadratic;
uniform float light_cutOff;
uniform float light_outerCutOff;
uniform DirLight dir_lights[10];
uniform SpotLight spot_lights[10];
uniform int num_dir_lights;
uniform int num_spot_lights;

vec3 calculate_dir_light(DirLight dir_light, vec3 normal, vec3 viewDir);
vec3 calculate_spot_light(SpotLight spot_light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main() {
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - fragPos);

	vec3 result = vec3(0, 0, 0);

	for(int i = 0; i < num_dir_lights; ++i)
		result += calculate_dir_light(dir_lights[i], norm, viewDir);
	for(int i = 0; i < num_spot_lights; ++i)
		result += calculate_spot_light(spot_lights[i], norm, fragPos, viewDir);

	fragColor = vec4(result, alpha_value);
}

vec3 calculate_dir_light(DirLight dir_light, vec3 normal, vec3 viewDir) {
	vec3 ambient = light_ambient * material.ambient;

	vec3 lightDir = normalize(dir_light.position - fragPos);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = light_diffuse * (diff * material.diffuse);

	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light_specular * (spec * material.specular);

	vec3 result = ambient + diffuse + specular;
	return result;
}

vec3 calculate_spot_light(SpotLight spot_light, vec3 normal, vec3 fragPos, vec3 viewDir) {
	vec3 lightDir = normalize(spot_light.position - fragPos);

	float diff = max(dot(normal, lightDir), 0.0);

	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

	float distance = length(spot_light.position - fragPos);
	float attenuation = 1.0 / (light_constant + light_linear * distance + light_quadratic * (distance * distance));    

	float theta = dot(lightDir, normalize(-spot_light.direction)); 
	float epsilon = light_cutOff - light_outerCutOff;
	float intensity = clamp((theta - light_outerCutOff) / epsilon, 0.0, 1.0);

	vec3 ambient = light_ambient * material.ambient;
	vec3 diffuse = light_diffuse * (diff * material.diffuse);
	vec3 specular = light_specular * (spec * material.specular);
	ambient *= attenuation * intensity;
	diffuse *= attenuation * intensity;
	specular *= attenuation * intensity;

	return (ambient + diffuse + specular);
}
