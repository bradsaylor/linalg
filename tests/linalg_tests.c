#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "linalg.h"

#define DELIM "********************************************\n"
#define TABLE_SIZE 256

#pragma region function prototypes
/* ============================================================================
 * Test function prototpes
 * ============================================================================
 */
int test_linalg_create_bind_matrix_00();
/*
int test_linalg_create_bind_matrix_01();
int test_linalg_create_bind_matrix_02();
int test_linalg_create_bind_matrix_03();
int test_linalg_create_bind_matrix_04();
int test_linalg_create_bind_matrix_05();
*/

/* ============================================================================
 * Helper function prototypes
 * ============================================================================
 */
int return_valid_matrix_components(struct List* elements, size_t* num_rows, size_t* num_cols);
int return_valid_vector_components(struct List* elements);
#pragma endregion

#pragma region main()
/* ============================================================================
 * main()
 * ============================================================================
 */
int main()
{
    assert(test_linalg_create_bind_matrix_00() == 0);
    /*
    assert(test_linalg_create_bind_matrix_01() == 0);
    assert(test_linalg_create_bind_matrix_02() == 0);
    assert(test_linalg_create_bind_matrix_03() == 0);
    assert(test_linalg_create_bind_matrix_04() == 0);
    assert(test_linalg_create_bind_matrix_05() == 0);
    */
   
    return 0;
}
#pragma endregion

#pragma region linalg_create_bind_matrix() tests
/* ============================================================================
 * linalg_create_bind_matrix() tests
 * ============================================================================
 */

int test_linalg_create_bind_matrix_00()
{
    // Test case for valid input

    const char* test_name = "test_linalg_create_bind_matrix_00";

    // create objects for test
    struct List elements = {0};
    size_t num_rows = 0;
    size_t num_cols = 0;
    return_valid_matrix_components(&elements, &num_rows, &num_cols);
    const char* name = "test";

    int rc = 1;

    do
    {
        // init reg_table
        bool init_table_OK = (linalg_init_reg_table(TABLE_SIZE) == 0);
        if (init_table_OK == false)
        {
            printf("%s FAILED on init_table_OK.\n%s\n", test_name, DELIM);
            break;
        }

        bool rtn_OK = (linalg_create_bind_matrix(elements, num_rows, num_cols, name) == 0);
        if (rtn_OK == false)
        {
            printf("%s FAILED on rtn_OK.\n%s\n", test_name, DELIM);
            free(elements.list);
            break;
        }

        printf("%s PASSED.\n%s\n", test_name, DELIM);
        rc = 0;
    } while (0);

    linalg_shutdown();
    return rc;
}
/*
int test_linalg_create_bind_matrix_01()
{
    // Violates condition:    1. name != NULL and name[0] != '\0'.

    const char* test_name = "test_linalg_create_bind_matrix_01";

    // create objects for test
    struct List elements = {0};
    int num_rows = 0;
    int num_cols = 0;
    return_valid_matrix_components(&elements, &num_rows, &num_cols);
    const char* name = "test";

    // init reg_table
    linalg_init_reg_table(TABLE_SIZE);
}

int test_linalg_create_bind_matrix_02()
{
    // Violates condition:    2. elements.list != NULL.

    const char* test_name = "test_linalg_create_bind_matrix_02";

    // create objects for test
    struct List elements = {0};
    int num_rows = 0;
    int num_cols = 0;
    return_valid_matrix_components(&elements, &num_rows, &num_cols);
    const char* name = "test";

    // init reg_table
    linalg_init_reg_table(TABLE_SIZE);
}

int test_linalg_create_bind_matrix_03()
{
    // Violates condition:    3. elements.type_size > 0.

    const char* test_name = "test_linalg_create_bind_matrix_03";

    // create objects for test
    struct List elements = {0};
    int num_rows = 0;
    int num_cols = 0;
    return_valid_matrix_components(&elements, &num_rows, &num_cols);
    const char* name = "test";

    // init reg_table
    linalg_init_reg_table(TABLE_SIZE);
}

int test_linalg_create_bind_matrix_04()
{
    // Violates condition:    4. num_rows > 0, num_cols > 0.

    const char* test_name = "test_linalg_create_bind_matrix_04";

    // create objects for test
    struct List elements = {0};
    int num_rows = 0;
    int num_cols = 0;
    return_valid_matrix_components(&elements, &num_rows, &num_cols);
    const char* name = "test";

    // init reg_table
    linalg_init_reg_table(TABLE_SIZE);
}

int test_linalg_create_bind_matrix_05()
{
    // Violates condition:    5. elements.size == num_rows * num_cols.

    const char* test_name = "test_linalg_create_bind_matrix_05";

    // create objects for test
    struct List elements = {0};
    int num_rows = 0;
    int num_cols = 0;
    return_valid_matrix_components(&elements, &num_rows, &num_cols);
    const char* name = "test";

    // init reg_table
    linalg_init_reg_table(TABLE_SIZE);
}
*/
#pragma endregion


#pragma region helper functions
/* ============================================================================
 * Helper functions
 * ============================================================================
 */

/*
  @brief
  Returns valid and consistent components for constructing matrix object.
  Allocates element_list which must be freed by caller.
  Valid components means `element_list_size` == `rows` * `cols`.
 */
int return_valid_matrix_components(struct List* elements, size_t* num_rows, size_t* num_cols)
{
    size_t element_list_size = 8;
    size_t rows = 4;
    size_t cols = 2;

    double list_to_copy[8] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0};
    double* element_list = malloc(element_list_size * sizeof(double));
    if (!element_list)
        return 2; // allocation failure

    memcpy(element_list, list_to_copy, sizeof(list_to_copy));

    elements->list = element_list;
    elements->size = element_list_size;
    elements->type_size = sizeof(double);

    *num_rows = rows;
    *num_cols = cols;

    return 0;
};

/*
  @brief
  Returns valid list for constructing vector object.
  Allocates element_list which must be freed by caller.
 */
int return_valid_vector_components(struct List* elements)
{
    size_t element_list_size = 8;

    double list_to_copy[8] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0};
    double* element_list = malloc(element_list_size * sizeof(double));
    if (!element_list)
        return 2; // allocation failure

    memcpy(element_list, list_to_copy, sizeof(list_to_copy));

    elements->list = element_list;
    elements->size = element_list_size;
    elements->type_size = sizeof(double);

    return 0;
};
#pragma endregion