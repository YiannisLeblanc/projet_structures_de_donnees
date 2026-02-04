#include "file_io/file.h"
#include <stdio.h>
#include <stdlib.h>

char *read_file(const char *file_name) {
    
    FILE *file = fopen(file_name, "rb");
    if (file == NULL) {
        printf("Error: file %s not found\n", file_name);
        return NULL;
    }
    fseek(file, 0, SEEK_END);
    const unsigned int file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *buffer = malloc((file_size + 1) * sizeof(char));
    fread(buffer, sizeof(char), file_size, file);
    fclose(file);
    buffer[file_size] = '\0';
    return buffer;
}