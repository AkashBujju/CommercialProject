#ifndef LIGHT_H
#define LIGHT_H

#define MAX_DIR_LIGHTS 25

#include <glad/glad.h>
#include "math.h"

typedef struct InstancedDirLight {
	GLuint program;
	GLuint vao, vbo, instanced_vbo;
	Matrix4 models[MAX_DIR_LIGHTS];
	Vector3 positions[MAX_DIR_LIGHTS];
	Vector3 scales[MAX_DIR_LIGHTS];
	uint32_t num_vertices;
	uint32_t num_models;
} InstancedDirLight;

void load_instanced_dir_light(InstancedDirLight *light, GLuint program, const char* model_filename, uint32_t num_models);
void translate_instanced_dir_light(InstancedDirLight *light, uint32_t model_index, float x, float y, float z);
void scale_instanced_dir_light(InstancedDirLight *light, uint32_t model_index, float x, float y, float z);
void draw_instanced_dir_light(InstancedDirLight *light);

#endif
