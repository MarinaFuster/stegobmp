#include <stdio.h>
#include <stdlib.h>

#include "include/bmp.h"

#define HEADER_SIZE 54

void free_header(bmp_header* header) {
    free(header);
}

void free_pixel_matrix(pixel*** matrix, unsigned int height, unsigned int width) {
    for(int i=0; i<height; i++) {
        for(int j=0; j<width;j++){
            free(matrix[i][j]);
        }
        free(matrix[i]);
    }
    free(matrix);
}

void free_information(information* info) {
    free_pixel_matrix(info->matrix, info->header->bmp_height, info->header->bmp_width);
    free_header(info->header);
    free(info);
}

information* bmp_to_matrix(const char* filename) {
    FILE* file;
    file = fopen(filename, "r");
    if(file == NULL) {
        perror("Error opening file\n");
        exit(1);
    }

    bmp_header* header = malloc(sizeof(*header));
    fread(header,sizeof(*header),1,file);

    unsigned int height = header->bmp_height;
    unsigned int width = header->bmp_width;
    pixel*** matrix = malloc(sizeof(*matrix)*height);
    for(int i=0; i<height; i++) {
        matrix[i] = malloc(sizeof(**matrix)*width);
        for(int j=0; j<width; j++) {
            matrix[i][j] = malloc(sizeof(pixel));
        }
    }
    for(int i=0; i<height; i++) {
        for(int j=0; j<width; j++) {
            fread(matrix[i][j],sizeof(pixel),1,file);
        }
    }
    information* info = malloc(sizeof(*info));
    info->matrix = matrix;
    info->header = header;
    return info;
}

/* file_name: must contain .bmp extension */
int matrix_to_bmp(information* info, char* file_name) {
    FILE* bmp_file = fopen(file_name, "w");
    if(bmp_file == NULL) {                                                  // Problems when opening the file...
        return 1;                                                           // error
    }
    unsigned int result = fwrite(info->header, HEADER_SIZE, 1, bmp_file);   // Writes header into file
    if(result != 1) {                                                       // If it didn't write what it needed to write...
        return 1;                                                           // error
    }
    unsigned int height = info->header->bmp_height;
    unsigned int width  = info->header->bmp_width;
    for(int i=0; i<height; i++) {
        for(int j=0; j<width; j++) {
            result = fwrite(info->matrix[i][j],sizeof(pixel), 1,bmp_file);  // Writes every pixel into file
            if(result != 1) {                                               // If it didn't write what it needed to write...
                return 1;                                                   // error
            }
        }
    }
    fclose(bmp_file);
    return 0;                                                               // success
}