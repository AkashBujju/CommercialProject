#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#define BUFFER_SIZE 32768

typedef struct Vector3 {
	float a, b, c;
} Vector3;

typedef struct Vector2 {
	float a, b;
} Vector2;

typedef struct Info {
	uint16_t vertex_numbers[4];
	uint16_t normal_numbers[4];
	uint8_t len; /* len may wary from 1 to 4 */
} Info;

typedef struct Strings {
	char strings[10][128];
	uint8_t len;
} Strings;

typedef struct SplitStrings {
	Strings *strings;
	uint32_t len;
} SplitStrings;

typedef struct Data {
	Vector3 vertices[1024];
	Vector3 vertex_normals[1024];
	Info infos[512];
	uint16_t v_len;
	uint16_t n_len;
	uint16_t i_len;
} Data;

void pack_into_structs(Data *data, SplitStrings *split_strs);

int main(int argc, char** argv) {
	if(argc != 2) {
		printf("Second argument ie. the filename is missing!!\n");
	}
	else {
		const size_t line_sz = 256;
		char buffer[line_sz];
		FILE *file = NULL;
		file = fopen(argv[1], "r");

		if(file == NULL) {
			printf("Could not open file %s\n", argv[1]);
		}
		else {
			fseek(file, 0, SEEK_END);
			const size_t filesize = ftell(file);
			fseek(file, 0, SEEK_SET);

			SplitStrings split_strs;
			split_strs.len = 0;
			split_strs.strings = (Strings*)malloc(sizeof(Strings) * BUFFER_SIZE);

			printf("-- Parsing file '%s' of size: %zu\n", argv[1], filesize);
			while(fgets(buffer, line_sz, file)) {
				/* Deleting the newline character */
				const size_t len = strlen(buffer);
				if(buffer[len - 1] == '\n')
					buffer[len - 1] = '\0';
				/* Deleting the newline character */

				/* Split string and Copy*/
				uint8_t str_index = 0;
				char* pch = strtok(buffer, " ");
				while(pch != NULL) {
					strcpy(split_strs.strings[split_strs.len].strings[str_index++], pch);
					pch = strtok(NULL, " ");
				}
				/* Split string and Copy*/

				split_strs.strings[split_strs.len].len = str_index;
				split_strs.len += 1;
			}

			Data data;
			pack_into_structs(&data, &split_strs);
			free(split_strs.strings);
		}

		fclose(file);
	}

	printf("-- ENDED\n");
	return 0;
}

void pack_into_structs(Data *data, SplitStrings *split_strs) {
	data->v_len = 0;
	data->n_len = 0;
	data->i_len = 0;

	for(uint32_t i = 0; i < split_strs->len; ++i) {
		char* str = split_strs->strings[i].strings[0];
		if(strcmp(str, "v") == 0) {
			float f1 = strtof(split_strs->strings[i].strings[1], NULL);
			float f2 = strtof(split_strs->strings[i].strings[2], NULL);
			float f3 = strtof(split_strs->strings[i].strings[3], NULL);
			data->vertices[data->v_len].a = f1;
			data->vertices[data->v_len].b = f2;
			data->vertices[data->v_len].c = f3;
			data->v_len += 1;
		}
		else if(strcmp(str, "vn") == 0) {
			float f1 = strtof(split_strs->strings[i].strings[1], NULL);
			float f2 = strtof(split_strs->strings[i].strings[2], NULL);
			float f3 = strtof(split_strs->strings[i].strings[3], NULL);
			data->vertex_normals[data->n_len].a = f1;
			data->vertex_normals[data->n_len].b = f2;
			data->vertex_normals[data->n_len].c = f3;
			data->n_len += 1;
		}
		else if(strcmp(str, "f") == 0) {
			data->infos[data->i_len].len = split_strs->strings[i].len - 1;
			for(uint8_t j = 1; j < split_strs->strings[i].len; ++j) {
				/* Split string */
				char *s = split_strs->strings[i].strings[j];
				char* s1 = strtok(s, "/");
				char* s2 = strtok(NULL, "/");
				char* s3 = strtok(NULL, "/");

				uint16_t vertex_index = strtof(s1, NULL);
				uint16_t normals_index = strtof(s3, NULL);
				data->infos[data->i_len].vertex_numbers[j - 1] = vertex_index;
				data->infos[data->i_len].normal_numbers[j - 1] = normals_index;
				/* Split string */
			}
			data->i_len += 1;
		}
	}

	for(uint16_t i = 0; i < data->v_len; ++i) {
		printf("v: %.2f %.2f %.2f\n", data->vertices[i].a, data->vertices[i].b, data->vertices[i].c);
	}
	printf("\n");
	for(uint16_t i = 0; i < data->n_len; ++i) {
		printf("n: %.2f %.2f %.2f\n", data->vertex_normals[i].a, data->vertex_normals[i].b, data->vertex_normals[i].c);
	}
	printf("\n");
	for(uint16_t i = 0; i < data->i_len; ++i) {
		for(uint8_t j = 0; j < data->infos[i].len; ++j)
			printf("v: %d ", data->infos[i].vertex_numbers[j]);
		printf("\n");
		for(uint8_t j = 0; j < data->infos[i].len; ++j)
			printf("n: %d ", data->infos[i].normal_numbers[j]);
		printf("\n");
	}
}
