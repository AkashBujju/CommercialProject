#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include "math.h"

typedef struct Material {
	Vector3 ambient;
	Vector3 diffuse;
	Vector3 specular;
	float shininess;
	uint8_t exists;
} Material;

int read_file(const char* filename, char *buffer);
char* combine_string(const char* str_1, const char* str_2);
void insert_char_into_string(char* src, char ch, uint16_t index);
void read_floats_from_file(const char* filename, float *buffer);
Material get_material_info_from_file(const char* filename, char* material_name);

#endif
