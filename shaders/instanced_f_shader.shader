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
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct SpotLight {
	vec3 position;  
   vec3 direction;
   float cutOff;
   float outerCutOff;
  
   vec3 ambient;
   vec3 diffuse;
   vec3 specular;
  
   float constant;
   float linear;
   float quadratic;
};

in vec3 Normal;
in vec3 fragPos;
in Material material;

uniform vec3 viewPos;
uniform DirLight dir_lights[2];
uniform SpotLight spot_lights[2];

vec3 calculate_dir_light(DirLight dir_light, vec3 normal, vec3 viewDir);
vec3 calculate_spot_light(SpotLight spot_light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main() {
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - fragPos);

	vec3 result = vec3(0, 0, 0);

	for(int i = 0; i < 2; ++i)
		result += calculate_dir_light(dir_lights[i], norm, viewDir);
	for(int i = 0; i < 2; ++i)
		result += calculate_spot_light(spot_lights[i], norm, fragPos, viewDir);

	fragColor = vec4(result, 1.0);
}

vec3 calculate_dir_light(DirLight dir_light, vec3 normal, vec3 viewDir) {
	vec3 ambient = dir_light.ambient * material.ambient;

	vec3 lightDir = normalize(dir_light.position - fragPos);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = dir_light.diffuse * (diff * material.diffuse);

	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = dir_light.specular * (spec * material.specular);

	vec3 result = ambient + diffuse + specular;
	return result;
}

vec3 calculate_spot_light(SpotLight spot_light, vec3 normal, vec3 fragPos, vec3 viewDir) {
	vec3 lightDir = normalize(spot_light.position - fragPos);

	float diff = max(dot(normal, lightDir), 0.0);

	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

	float distance = length(spot_light.position - fragPos);
	float attenuation = 1.0 / (spot_light.constant + spot_light.linear * distance + spot_light.quadratic * (distance * distance));    

	float theta = dot(lightDir, normalize(-spot_light.direction)); 
	float epsilon = spot_light.cutOff - spot_light.outerCutOff;
	float intensity = clamp((theta - spot_light.outerCutOff) / epsilon, 0.0, 1.0);

	vec3 ambient = spot_light.ambient * material.ambient;
	vec3 diffuse = spot_light.diffuse * (diff * material.diffuse);
	vec3 specular = spot_light.specular * (spec * material.specular);
	ambient *= attenuation * intensity;
	diffuse *= attenuation * intensity;
	specular *= attenuation * intensity;

	return (ambient + diffuse + specular);
}
