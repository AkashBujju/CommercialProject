#ifndef INSTANCED_MODEL_H
#define INSTANCED_MODEL_H

#define MAX_INSTANCED_MODELS 25

#include <glad/glad.h>
#include "math.h"
#include "light.h"

typedef struct BoundingBox {
	float width, height, depth;
} BoundingBox;

typedef struct InstancedModel {
	GLuint program, vao;
	GLuint instanceAmbientVBO;
	GLuint instanceDiffuseVBO;
	GLuint instanceSpecularVBO;
	GLuint instanceShininessVBO;
	GLuint instanceAlphaVBO;
	GLuint instanceModelVBO;
	Matrix4 models[MAX_INSTANCED_MODELS];
	Vector3 positions[MAX_INSTANCED_MODELS];
	Vector3 initial_positions[MAX_INSTANCED_MODELS];
	Vector3 scales[MAX_INSTANCED_MODELS];
	BoundingBox bounding_boxes[MAX_INSTANCED_MODELS];
	BoundingBox default_boxes[MAX_INSTANCED_MODELS];
	Vector3 rotation_axes[MAX_INSTANCED_MODELS];
	Vector3 initial_rotation_axes[MAX_INSTANCED_MODELS];
	Vector3 rotation_points[MAX_INSTANCED_MODELS];
	Vector3 ambient[MAX_INSTANCED_MODELS];
	Vector3 diffuse[MAX_INSTANCED_MODELS];
	Vector3 specular[MAX_INSTANCED_MODELS];
	float shininess[MAX_INSTANCED_MODELS];
	float alpha[MAX_INSTANCED_MODELS];
	float angle_in_degree[MAX_INSTANCED_MODELS];
	float initial_angle_in_degree[MAX_INSTANCED_MODELS];
	uint32_t num_models;
	uint32_t num_vertices;
	char model_name[50];
} InstancedModel;

void load_instanced_model(InstancedModel *instanced_model, GLuint program, char *model_name, const char* model_filename);
void add_model(InstancedModel *instanced_model, float x, float y, float z, char *material_name);
void set_initial_pose_instanced_model(InstancedModel *instanced_model, uint32_t model_index, float px, float py, float pz, float axis_x, float axis_y, float axis_z, float angle);
void translate_instanced_model(InstancedModel *instanced_model, uint32_t model_index, float x, float y, float z);
void scale_instanced_model(InstancedModel *instanced_model, uint32_t model_index, float x, float y, float z);
void rotate_instanced_model_about(InstancedModel *instanced_model, uint32_t model_index, float axis_x, float axis_y, float axis_z, float about_x, float about_y, float about_z, float degree);
void set_material_instanced_model(InstancedModel *instanced_model, uint32_t model_index, char* material_name);
void set_alpha_instanced_model(InstancedModel *instanced_model, uint32_t model_index, float alpha_value);
void draw_instanced_model(InstancedModel *instanced_model, InstancedDirLight *instanced_dir_light, InstancedSpotLight *instanced_spot_light);

#endif
