#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ERR_MSG "[error]\n"
#define ERR_CODE -1
#define NORMAL_CODE 0

typedef struct Lines {
	char** data;
	size_t count;
	size_t capacity;
} Lines;

Lines* init_lines() {
	Lines* lines = malloc(sizeof(Lines));
	if (!lines) {
		return NULL;
	}
	lines->count = 0;
	lines->capacity = 1;
	lines->data = (char**) malloc(sizeof(char*) * lines->capacity);
	if (!lines->data) {
		free(lines);
		return NULL;
	}
	return lines;
}

void free_lines(Lines* lines) {
	if (!lines) {
		return;
	}
	for (int i = 0; i < lines->count; i++) {
		free(lines->data[i]);
	}
	free(lines->data);
	free(lines);
	return;
}

int add(Lines* lines, const char* line) {
	if (lines->capacity == lines->count) {
		lines->capacity = lines->capacity * 2;
		lines->data = (char**) realloc(lines->data, sizeof(char*) * lines->capacity);
		if (!lines->data) {
			free_lines(lines);
			return ERR_CODE;
		}
	}
	lines->data[lines->count++] = strdup(line);
	return NORMAL_CODE;
}

void print_lines(Lines* lines) {
	for (int i = 0; i < lines-> count; i++) {
		printf("%s", lines->data[i]);
	}
}

int to_lowercase(Lines* input, Lines* lowercase) {
	for (int i = 0; i < input->count; i++) {
		int j = 0;
		char* current_line = (char*) malloc(sizeof(char) * (strlen(input->data[i]) + 1));
		if (!current_line) {
			return ERR_CODE;
		}
		while (input->data[i][j] != '\0') {
			if ('A' <= input->data[i][j] && input->data[i][j] <= 'Z') {
				current_line[j] = input->data[i][j] + 'a' - 'A';
			} else {
				current_line[j] = input->data[i][j];
			}
			j++;
		}
		current_line[j++] = '\0';
		add(lowercase, current_line);
		free(current_line);
	}
	return lowercase->count;
}

int main() {
	Lines* lines = init_lines();
	if (!lines) {
		printf(ERR_MSG);
		return 0;
	}
	char* line = NULL;
	size_t len = 0;
	while (getline(&line, &len, stdin) != -1){
		add(lines, line);
	}
	free(line);
	Lines* lowercase = init_lines();
	if (!lowercase) {
		free_lines(lines);
		printf(ERR_MSG);
		return 0;
	}
	if (to_lowercase(lines, lowercase) == ERR_CODE) {
		printf(ERR_MSG);
		return 0;
	}
	print_lines(lowercase);
	free_lines(lines);
	free_lines(lowercase);
	return 0;
}