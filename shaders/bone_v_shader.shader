#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aBoneIndices;
layout (location = 2) in vec2 aWeights;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 bones[10];

void main() {
	int index_1 = int(aBoneIndices.x);
	int index_2 = int(aBoneIndices.y);
	mat4 bone_transform = bones[index_1] * aWeights.x;
	bone_transform += bones[index_2] * aWeights.y;

	gl_Position = projection * view * model * bone_transform * vec4(aPos, 1.0f);
}
