#ifndef INSTANCED_MODEL_H
#define INSTANCED_MODEL_H

#define MAX_MODELS 25

#include <glad/glad.h>
#include "math.h"

typedef struct InstancedModel {
	GLuint program, vao, vbo;
	GLuint instanceAmbientVBO;
	GLuint instanceDiffuseVBO;
	GLuint instanceSpecularVBO;
	GLuint instanceShininessVBO;
	GLuint instanceModelVBO;
	Matrix4 models[MAX_MODELS];
	Vector3 positions[MAX_MODELS];
	Vector3 scales[MAX_MODELS];
	Vector3 rotation_axes[MAX_MODELS];
	Vector3 ambient[MAX_MODELS];
	Vector3 diffuse[MAX_MODELS];
	Vector3 specular[MAX_MODELS];
	float shininess[MAX_MODELS];
	float angle_in_degree[MAX_MODELS];
	float width[MAX_MODELS];
	float height[MAX_MODELS];
	float depth[MAX_MODELS];
	uint32_t num_models;
	uint32_t num_vertices;
} InstancedModel;

void load_instanced_model(InstancedModel *instanced_model, GLuint program, const char* model_filename, uint32_t num_models);
void translate_instanced_model(InstancedModel *instanced_model, uint32_t model_index, float x, float y, float z);
void scale_instanced_model(InstancedModel *instanced_model, uint32_t model_index, float x, float y, float z);
void rotate_instanced_model(InstancedModel *instanced_model, uint32_t model_index, float x, float y, float z, float degree);
void set_material_instanced_model(InstancedModel *instanced_model, uint32_t model_index, char* material_name);
void draw_instanced_model(InstancedModel *instanced_model);

#endif
