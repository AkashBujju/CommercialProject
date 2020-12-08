#ifndef HELPER_MODELS_H
#define HELPER_MODELS_H

#define MAX_INSTANCED_HELPER_MODELS 25

#include <glad/glad.h>
#include "../../src/math.h"
#include "../../src/instanced_model.h"

typedef struct InstancedHelperModel {
	GLuint program, vao, vbo;
	GLuint instanceModelVBO;
	GLuint instanceColorVBO;
	Matrix4 models[MAX_INSTANCED_HELPER_MODELS];
	Vector3 positions[MAX_INSTANCED_HELPER_MODELS];
	Vector3 scales[MAX_INSTANCED_HELPER_MODELS];
	Vector3 rotation_axes[MAX_INSTANCED_HELPER_MODELS];
	Vector3 colors[MAX_INSTANCED_HELPER_MODELS];
	float angle_in_degree[MAX_INSTANCED_HELPER_MODELS];
	BoundingBox bounding_boxes[MAX_INSTANCED_HELPER_MODELS];
	uint32_t num_models;
	uint32_t num_vertices;
} InstancedHelperModel;

void load_instanced_helper_model(InstancedHelperModel *instanced_helper_model, GLuint program, const char* model_filename);
void add_helper_model(InstancedHelperModel *instanced_helper_model, float x, float y, float z);
void draw_instanced_helper_model(InstancedHelperModel *instanced_helper_model, InstancedDirLight *instanced_dir_light);
void rotate_instanced_helper_model(InstancedHelperModel *instanced_helper_model, uint32_t model_index, float x, float y, float z, float degree);
void scale_instanced_helper_model(InstancedHelperModel *instanced_helper_model, uint32_t model_index, float x, float y, float z);
void translate_instanced_helper_model(InstancedHelperModel *instanced_helper_model, uint32_t model_index, float x, float y, float z);
void set_color_instanced_helper_model(InstancedHelperModel *instanced_helper_model, uint32_t model_index, float r, float g, float b);

typedef struct HelperModels {
	InstancedHelperModel cubes;	
} HelperModels;

#endif
