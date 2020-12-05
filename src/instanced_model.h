#ifndef INSTANCED_MODEL_H
#define INSTANCED_MODEL_H

#define MAX_MODELS 25

#include <glad/glad.h>
#include "math.h"
#include "light.h"

typedef struct BoundingBox {
	float width, height, depth;
} BoundingBox;

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
	BoundingBox bounding_boxes[MAX_MODELS];
	Vector3 rotation_axes[MAX_MODELS];
	Vector3 ambient[MAX_MODELS];
	Vector3 diffuse[MAX_MODELS];
	Vector3 specular[MAX_MODELS];
	float shininess[MAX_MODELS];
	float angle_in_degree[MAX_MODELS];
	uint32_t num_models;
	uint32_t num_vertices;
	char model_name[50];
} InstancedModel;

void load_instanced_model(InstancedModel *instanced_model, GLuint program, char *model_name, const char* model_filename);
void add_model(InstancedModel *instanced_model, float x, float y, float z, char *material_name);
void translate_instanced_model(InstancedModel *instanced_model, uint32_t model_index, float x, float y, float z);
void move_instanced_model_along(InstancedModel *instanced_model, uint32_t model_index, Vector *ray, uint8_t along_x, uint8_t along_y, uint8_t along_z);
void scale_instanced_model_along(InstancedModel *instanced_model, uint32_t model_index, Vector *ray, uint8_t along_x, uint8_t along_y, uint8_t along_z);
void scale_instanced_model(InstancedModel *instanced_model, uint32_t model_index, float x, float y, float z);
void rotate_instanced_model(InstancedModel *instanced_model, uint32_t model_index, float x, float y, float z, float degree);
void set_material_instanced_model(InstancedModel *instanced_model, uint32_t model_index, char* material_name);
void draw_instanced_model(InstancedModel *instanced_model, InstancedDirLight *instanced_dir_light, InstancedSpotLight *instanced_spot_light);

#endif
