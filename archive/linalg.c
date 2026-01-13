#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// constants

#define MAT_LIST_SIZE_INC 8  // increment size when resizing matrix list

// data structures

struct Matrix {
  char* name;
  int num_rows;
  int num_columns;
  float* elements;
};

struct Matrix_list {  // Data structure to keep list of allocated matrices
  struct Matrix** m_list;
  int size;
  int num_mats;
};

struct Test_data {
  char* name;
  float* elements;
  int num_rows;
  int num_cols;
};

// function declarations

struct Matrix_list* alloc_init_matrix_list(struct Matrix** init_matrix_list);
float* alloc_array(float* elements, int size);
char* alloc_name(char* name, int size);
struct Matrix* alloc_mat(char* name, int num_rows, int num_columns,
                         float* elements);
struct Matrix_list* matrix_list_add(struct Matrix_list* mat_list,
                                    struct Matrix* new_matrix);
int print_matrix(struct Matrix* matrix);
int free_matrix_list(struct Matrix_list* mat_list);
struct Matrix_list* add_test_data(float* elements, char* name, int num_rows,
                                  int num_cols);

// test data

float test_elements1[] = {1, 2, 3, 4, 5};
char test_name1[] = "a_matrix";
float test_elements2[] = {11, 22, 33, 44, 55};
char test_name2[] = "b_matrix";
float test_elements3[] = {15, 25, 35, 45, 55};
char test_name3[] = "c_matrix";
float test_elements4[] = {100, 200, 300, 400, 500};
char test_name4[] = "d_matrix";
float test_elements5[] = {0.1, 0.2, 0.3, 0.4, 0.5};
char test_name5[] = "e_matrix";
struct Test_data test_data[] = {{.name = test_name1,
                                 .elements = test_elements1,
                                 .num_rows = 5,
                                 .num_cols = 1},

                                {.name = test_name2,
                                 .elements = test_elements2,
                                 .num_rows = 5,
                                 .num_cols = 1},

                                {.name = test_name3,
                                 .elements = test_elements3,
                                 .num_rows = 5,
                                 .num_cols = 1},

                                {.name = test_name4,
                                 .elements = test_elements4,
                                 .num_rows = 5,
                                 .num_cols = 1},

                                {.name = test_name5,
                                 .elements = test_elements5,
                                 .num_rows = 5,
                                 .num_cols = 1}};

// MAIN

int main() {
  // Initialize matrix list with empty m_list
  struct Matrix* init_matrix_list[1] = {0};
  struct Matrix_list* matrix_list = alloc_init_matrix_list(init_matrix_list);

  for (int i = 0; i < 2; i++) {
    struct Test_data temp = test_data[i];
    matrix_list =
        add_test_data(temp.elements, temp.name, temp.num_rows, temp.num_cols);
  }
  for (int i = 0; i < matrix_list->num_mats; i++) {
    print_matrix(matrix_list->m_list[i]);
  }
  free_matrix_list(matrix_list);

  return 0;
}

// function definitions

struct Matrix_list* alloc_init_matrix_list(struct Matrix** init_matrix_list) {
  struct Matrix_list* matrix_list_ptr = malloc(sizeof(struct Matrix_list));
  matrix_list_ptr->m_list = init_matrix_list;
  matrix_list_ptr->num_mats = 0;
  matrix_list_ptr->size = 0;

  return matrix_list_ptr;
}

float* alloc_array(float* elements, int size) {
  // allocate space for new array and copy in data
  float* flt_ptr = malloc(size * sizeof(float));
  memcpy(flt_ptr, elements, size * sizeof(float));

  return flt_ptr;
}

char* alloc_name(char* name, int size) {
  // allocate space for new name string and copy in data
  char* char_ptr = malloc(size * sizeof(float));
  memcpy(char_ptr, name, size * sizeof(char));

  return char_ptr;
}

struct Matrix* alloc_mat(char* name, int num_rows, int num_columns,
                         float* elements) {
  // allcate space for new matrix and copy in data
  struct Matrix* mat_ptr = malloc(sizeof(struct Matrix));
  mat_ptr->name = name;
  mat_ptr->num_rows = num_rows;
  mat_ptr->num_columns = num_columns;
  mat_ptr->elements = elements;

  return mat_ptr;
}

struct Matrix_list* matrix_list_add(struct Matrix_list* mat_list,
                                    struct Matrix* new_matrix) {
  // if list is full increment list size with reallocate
  if (mat_list->num_mats == mat_list->size) {
    mat_list = realloc(
        mat_list, mat_list->size + MAT_LIST_SIZE_INC * sizeof(struct Matrix*));
    mat_list->size += MAT_LIST_SIZE_INC;
  }
  // register new matrix at index [size]
  mat_list->m_list[mat_list->num_mats] = new_matrix;
  // increment size
  (mat_list->num_mats)++;

  return mat_list;
}

int print_matrix(struct Matrix* matrix) {
  printf("\n[");
  for (int i = 0; i < matrix->num_rows; i++) {
    printf("\n\t[");
    for (int j = 0; j < matrix->num_columns; j++) {
      printf("%f", matrix->elements[i * matrix->num_columns + j]);
      if (j != matrix->num_columns - 1) printf(", ");
    }
    printf("]\n");
  }
  printf("]\n");

  return 0;
}

int free_matrix_list(struct Matrix_list* mat_list) {
  for (int i = 0; i < mat_list->num_mats; i++) {
    free(mat_list->m_list[i]->name);
    free(mat_list->m_list[i]->elements);
    free(mat_list->m_list[i]);
  }

  free(mat_list);

  return 0;
}

struct Matrix_list* add_test_data(float* elements, char* name, int num_rows,
                                  int num_cols) {
  float* new_elements = alloc_array(elements, num_rows * num_cols);
  char* new_name = alloc_name(name, strlen(name));
  struct Matrix* matrix = alloc_mat(new_name, num_rows, num_cols, new_elements);
  struct Matrix_list* matrix_list = matrix_list_add(matrix_list, matrix);
  return matrix_list;
}