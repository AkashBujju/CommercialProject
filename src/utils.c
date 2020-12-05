#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "math.h"

int read_file(const char* filename, char *buffer) {
	FILE *file = NULL;
	file = fopen(filename, "r");
	if(file == NULL) {
		printf("Could not open file %s\n", filename);
		return -1;
	}

	fseek(file, 0, SEEK_END);
	const size_t filesize = ftell(file);
	fseek(file, 0, SEEK_SET);

	fread(buffer, 1, filesize, file);
	buffer[filesize] = 0;
	
	fclose(file);
	return 0;
}

void get_model_data_from_file(const char* filename, Vector3 *extreme_mins, Vector3 *extreme_maxs, float *vertices_buffer) {
	FILE *file = NULL;
	file = fopen(filename, "r");
	if(file == NULL) {
		printf("ERROR get_model_data_from_file(): Could not open file %s\n", filename);
	}

	char num_sub_models_str[25];
	fscanf(file, "%s", num_sub_models_str);
	uint16_t num_sub_models;
	fscanf(file, "%hu", &num_sub_models);

	char num_vertex_floats_str[25];
	fscanf(file, "%s", num_vertex_floats_str);
	uint32_t num_floats_in_file;
	fscanf(file, "%u", &num_floats_in_file);

	char extremes_str[25];
	fscanf(file, "%s", extremes_str);
	fscanf(file, "%f", &extreme_mins->x);
	fscanf(file, "%f", &extreme_maxs->x);
	fscanf(file, "%f", &extreme_mins->y);
	fscanf(file, "%f", &extreme_maxs->y);
	fscanf(file, "%f", &extreme_mins->z);
	fscanf(file, "%f", &extreme_maxs->z);

	for(int i = 0; i < num_floats_in_file; ++i)
		fscanf(file, "%f", &vertices_buffer[i]);
}

void read_floats_from_file(const char* filename, float *buffer) {
	FILE *file = NULL;
	file = fopen(filename, "r");
	if(file == NULL) {
		printf("Could not open file %s\n", filename);
		return;
	}

	int num_values;
	fscanf(file, "%d", &num_values);

	for(int i = 0; i < num_values; ++i)
		fscanf(file, "%f", buffer + i);

	fclose(file);
}

Material get_material_info_from_file(const char* filename, char* material_name) {
	Material material;
	material.exists = 0;

	FILE *file = NULL;
	file = fopen(filename, "r");
	if(file == NULL) {
		printf("Could not open file %s\n", filename);
	}
	else {
		int num_materials;
		char tmp_buffer[25];
		fscanf(file, "%s", tmp_buffer);
		fscanf(file, "%d", &num_materials);

		for(int i = 0; i < num_materials; ++i) {
			fscanf(file, "%s", tmp_buffer); /* nm */
			char name_in_file[25];
			fscanf(file, "%s", name_in_file);

			Vector3 vector[3];
			for(uint8_t i = 0; i < 3; ++i) {
				fscanf(file, "%s", tmp_buffer);
				fscanf(file, "%f", &vector[i].x);
				fscanf(file, "%f", &vector[i].y);
				fscanf(file, "%f", &vector[i].z);
			}

			copy_vector(&material.ambient, &vector[0]);
			copy_vector(&material.diffuse, &vector[1]);
			copy_vector(&material.specular, &vector[2]);

			fscanf(file, "%s", tmp_buffer); /* shininess */
			fscanf(file, "%f", &material.shininess);

			if(strcmp(name_in_file, material_name) == 0) {
				material.exists = 1;
				break;
			}
		}

		if(material.exists == 0) {
			printf("WARNING: Material %s not found in file: %s\n", material_name, filename);
		}

		fclose(file);
	}

	return material;
}

void insert_char_into_string(char* src, char ch, uint16_t index) {
	uint16_t len = strlen(src);
	if(index == len) {
		src[index] = ch;
		src[index + 1] = '\0';
	}
	else if(index > len - 1) {
		printf("WARNING insert_char_into_string(): index(%d) >= len(%d).\n", index, len);
	}
	else {
		for(uint16_t i = len - 1; i >= index && i <= len - 1; --i)
			src[i + 1] = src[i];

		src[index] = ch;
		src[len + 1] = '\0';
	}
}

char* combine_string(const char* str_1, const char* str_2) {
	unsigned int len = 100;
	char* str = (char*)malloc(sizeof(char) * len);

	strcpy(str, str_1);
	strcat(str, str_2);

	return str;
}
