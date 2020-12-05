#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <float.h>

#define BUFFER_SIZE 32768
#define ARRAY_SIZE 8192

typedef struct Vector3 {
	float a, b, c;
} Vector3;

typedef struct Vector2 {
	float a, b;
} Vector2;

typedef struct Info {
	uint16_t vertex_numbers[4];
	uint16_t normal_numbers[4];
	uint16_t len; /* len may wary from 1 to 4 */
} Info;

typedef struct Strings {
	char strings[10][128];
	uint16_t len;
} Strings;

typedef struct SplitStrings {
	Strings *strings;
	uint32_t len;
} SplitStrings;

typedef struct Data {
	Vector3 vertices[ARRAY_SIZE];
	Vector3 vertex_normals[ARRAY_SIZE];
	Info infos[ARRAY_SIZE];
	uint32_t v_len;
	uint32_t n_len;
	uint32_t i_len;
} Data;

#define MAX_SEPERATE_MODELS 50
typedef struct Model {
	Data *data;
	uint16_t count;
	float min_x, max_x;
	float min_y, max_y;
	float min_z, max_z;
} Model;

void pack_into_structs(Model *model, SplitStrings *split_strs);
void save_model(Model *model, const char*);

int main(int argc, char** argv) {
	if(argc != 2) {
		printf("Second argument ie. the filename is missing!!\n");
	}
	else {
#define LINE_SIZE 256
		char buffer[LINE_SIZE];
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

			printf("-- Parsing file '%s' of size: %zu bytes\n", argv[1], filesize);

			Model model;
			model.data = (Data*)malloc(sizeof(Data) * MAX_SEPERATE_MODELS);
			model.count = 0;
			while(fgets(buffer, LINE_SIZE, file)) {
#undef LINE_SIZE
				/* Deleting the newline character */
				const size_t len = strlen(buffer);
				if(buffer[len - 1] == '\n')
					buffer[len - 1] = '\0';
				/* Deleting the newline character */

				/* Split string and Copy */
				uint16_t str_index = 0;
				char* pch = strtok(buffer, " ");

				/* Checking for new set of vertices */
				if(strcmp(pch, "start") == 0) {
					model.count += 1;
				}
				/* Checking for new set of vertices */

				while(pch != NULL) {
					strcpy(split_strs.strings[split_strs.len].strings[str_index++], pch);
					/* @Note: How is this not crashing for cone.obj, for array size > 10 */
					pch = strtok(NULL, " ");
				}
				/* Split string and Copy */

				if(model.count > MAX_SEPERATE_MODELS) {
					printf("-- WARNING: wav_to_model -> model.count(%hu) > MAX_SEPERATE_MODELS(%d)\n", model.count, MAX_SEPERATE_MODELS);
					printf("-- QUIT wav_to_model file generation abruptly!!!!\n\n");
					return -1;
				}
#undef MAX_SEPERATE_MODELS

				split_strs.strings[split_strs.len].len = str_index;
				split_strs.len += 1;
			}

			pack_into_structs(&model, &split_strs);
			free(split_strs.strings);

			/* Saving */
			char* pch = strtok(argv[1], ".");
#ifdef __APPLE__
			pch = strtok(pch, "/");
			pch = strtok(NULL, "/");
			char filename[50] = "../data_out/";
#else
			pch = strtok(pch, "\\");
			pch = strtok(NULL, "\\");
			char filename[50] = "..\\data_out\\";
#endif
			strcat(filename, pch);
			strcat(filename, ".model");
			save_model(&model, filename);
			/* Saving */
		}

		fclose(file);
	}

	printf("-- ENDED\n\n");
	return 0;
}

void pack_into_structs(Model *model, SplitStrings *split_strs) {
	int16_t di = -1;

	model->min_x = +FLT_MAX;
	model->max_x = -FLT_MAX;
	model->min_y = +FLT_MAX;
	model->max_y = -FLT_MAX;
	model->min_z = +FLT_MAX;
	model->max_z = -FLT_MAX;

	for(uint32_t i = 0; i < split_strs->len; ++i) {
		char* str = split_strs->strings[i].strings[0];
		if(strcmp(str, "start") == 0) {
			di += 1;
			model->data[di].v_len = 0;
			model->data[di].n_len = 0;
			model->data[di].i_len = 0;
		}
		else if(strcmp(str, "v") == 0) {
			float f1 = strtof(split_strs->strings[i].strings[1], NULL);
			float f2 = strtof(split_strs->strings[i].strings[2], NULL);
			float f3 = strtof(split_strs->strings[i].strings[3], NULL);
			model->data[di].vertices[model->data[di].v_len].a = f1;
			model->data[di].vertices[model->data[di].v_len].b = f2;
			model->data[di].vertices[model->data[di].v_len].c = f3;
			model->data[di].v_len += 1;

			if(f1 < model->min_x) model->min_x = f1;
			if(f1 > model->max_x) model->max_x = f1;
			if(f2 < model->min_y) model->min_y = f2;
			if(f2 > model->max_y) model->max_y = f2;
			if(f3 < model->min_z) model->min_z = f3;
			if(f3 > model->max_z) model->max_z = f3;
		}
		else if(strcmp(str, "vn") == 0) {
			float f1 = strtof(split_strs->strings[i].strings[1], NULL);
			float f2 = strtof(split_strs->strings[i].strings[2], NULL);
			float f3 = strtof(split_strs->strings[i].strings[3], NULL);
			model->data[di].vertex_normals[model->data[di].n_len].a = f1;
			model->data[di].vertex_normals[model->data[di].n_len].b = f2;
			model->data[di].vertex_normals[model->data[di].n_len].c = f3;
			model->data[di].n_len += 1;
		}
		else if(strcmp(str, "f") == 0) {
			/* Calculating number of vertices to subtract from infos index */
			uint32_t vertex_sub_count, normal_sub_count;
			if(di == 0) {
				vertex_sub_count = 0;
				normal_sub_count = 0;
			}
			else {
				vertex_sub_count = 0;
				normal_sub_count = 0;
				for(uint16_t x = 1; x <= di; ++x) {
					vertex_sub_count += model->data[x - 1].v_len;
					normal_sub_count += model->data[x - 1].n_len;
				}
			}
			/* Calculating number of vertices to subtract from infos index */

			model->data[di].infos[model->data[di].i_len].len = split_strs->strings[i].len - 1;
			for(uint16_t j = 1; j < split_strs->strings[i].len; ++j) {
				/* Split string */
				char *s = split_strs->strings[i].strings[j];
				char* s1 = strtok(s, "/");
				char* s2 = strtok(NULL, "/");
				char* s3 = strtok(NULL, "/");

				if(s3 == NULL) {
					s3 = s2;
					s2 = NULL;
				}

				uint16_t vertex_index = strtof(s1, NULL);
				uint16_t normals_index = strtof(s3, NULL);
				model->data[di].infos[model->data[di].i_len].vertex_numbers[j - 1] = vertex_index - vertex_sub_count;
				model->data[di].infos[model->data[di].i_len].normal_numbers[j - 1] = normals_index - normal_sub_count;
				/* Split string */
			}

			model->data[di].i_len += 1;
		}
	}
}

void save_model(Model *model, const char* filename) {
	FILE *file = fopen(filename, "w");
	if(!file) {
		printf("File %s not found\n", filename);
		return;
	}

	/* Calculating total number of floats */
	uint32_t num_floats = 0;
	for(uint16_t di = 0; di < model->count; ++di) {
		/* @Note: 3 for the co-ordinates and 3 for the normals := 6 */
		for(uint16_t i = 0; i < model->data[di].i_len; ++i) {
			if(model->data[di].infos[i].len == 4)
				num_floats += (model->data[di].infos[i].len + 2) * 6;
			else
				num_floats += model->data[di].infos[i].len * 6;
		}
	}
	/* Calculating total number of floats */

	fprintf(file, "num_sub_models %u", model->count);
	fprintf(file, "\nnum_vertex_floats %u", num_floats);

	/* Printing extremes */
	fprintf(file, "\n%s", "extremes");
	fprintf(file, " %+.3f %+.3f %+.3f %+.3f %+.3f %+.3f", model->min_x, model->max_x, model->min_y, model->max_y, model->min_z, model->max_z);
	/* Printing extremes */

	for(uint16_t di = 0; di < model->count; ++di) {
		for(uint32_t i = 0; i < model->data[di].i_len; ++i) {
			if(model->data[di].infos[i].len == 4) {
				uint16_t vi_1 = model->data[di].infos[i].vertex_numbers[0] - 1; 
				uint16_t vi_2 = model->data[di].infos[i].vertex_numbers[1] - 1;
				uint16_t vi_3 = model->data[di].infos[i].vertex_numbers[2] - 1;
				uint16_t vi_4 = model->data[di].infos[i].vertex_numbers[3] - 1;
				uint16_t ni_1 = model->data[di].infos[i].normal_numbers[0] - 1; 
				uint16_t ni_2 = model->data[di].infos[i].normal_numbers[1] - 1;
				uint16_t ni_3 = model->data[di].infos[i].normal_numbers[2] - 1;
				uint16_t ni_4 = model->data[di].infos[i].normal_numbers[3] - 1;
				Vector3 *v1 = &model->data[di].vertices[vi_1];
				Vector3 *v2 = &model->data[di].vertices[vi_2];
				Vector3 *v3 = &model->data[di].vertices[vi_3];
				Vector3 *v4 = &model->data[di].vertices[vi_4];
				Vector3 *n1 = &model->data[di].vertex_normals[ni_1];
				Vector3 *n2 = &model->data[di].vertex_normals[ni_2];
				Vector3 *n3 = &model->data[di].vertex_normals[ni_3];
				Vector3 *n4 = &model->data[di].vertex_normals[ni_4];

				fprintf(file, "\n%+.3f %+.3f %+.3f %+.3f %+.3f %+.3f", v1->a, v1->b, v1->c, n1->a, n1->b, n1->c);
				fprintf(file, "\n%+.3f %+.3f %+.3f %+.3f %+.3f %+.3f", v2->a, v2->b, v2->c, n2->a, n2->b, n2->c);
				fprintf(file, "\n%+.3f %+.3f %+.3f %+.3f %+.3f %+.3f", v3->a, v3->b, v3->c, n3->a, n3->b, n3->c);
				fprintf(file, "\n%+.3f %+.3f %+.3f %+.3f %+.3f %+.3f", v1->a, v1->b, v1->c, n1->a, n1->b, n1->c);
				fprintf(file, "\n%+.3f %+.3f %+.3f %+.3f %+.3f %+.3f", v4->a, v4->b, v4->c, n4->a, n4->b, n4->c);
				fprintf(file, "\n%+.3f %+.3f %+.3f %+.3f %+.3f %+.3f", v3->a, v3->b, v3->c, n3->a, n3->b, n3->c);
			}
			else if(model->data[di].infos[i].len == 3) {
				uint16_t vi_1 = model->data[di].infos[i].vertex_numbers[0] - 1; 
				uint16_t vi_2 = model->data[di].infos[i].vertex_numbers[1] - 1;
				uint16_t vi_3 = model->data[di].infos[i].vertex_numbers[2] - 1;
				uint16_t ni_1 = model->data[di].infos[i].normal_numbers[0] - 1; 
				uint16_t ni_2 = model->data[di].infos[i].normal_numbers[1] - 1;
				uint16_t ni_3 = model->data[di].infos[i].normal_numbers[2] - 1;
				Vector3 *v1 = &model->data[di].vertices[vi_1];
				Vector3 *v2 = &model->data[di].vertices[vi_2];
				Vector3 *v3 = &model->data[di].vertices[vi_3];
				Vector3 *n1 = &model->data[di].vertex_normals[ni_1];
				Vector3 *n2 = &model->data[di].vertex_normals[ni_2];
				Vector3 *n3 = &model->data[di].vertex_normals[ni_3];

				fprintf(file, "\n%+.3f %+.3f %+.3f %+.3f %+.3f %+.3f", v1->a, v1->b, v1->c, n1->a, n1->b, n1->c);
				fprintf(file, "\n%+.3f %+.3f %+.3f %+.3f %+.3f %+.3f", v2->a, v2->b, v2->c, n2->a, n2->b, n2->c);
				fprintf(file, "\n%+.3f %+.3f %+.3f %+.3f %+.3f %+.3f", v3->a, v3->b, v3->c, n3->a, n3->b, n3->c);
			}
		}
	}

	fclose(file);
	printf("-- Saved to %s\n", filename);
}
