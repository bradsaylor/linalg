#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "linalg.h"
#include "logs.h"

#define DELIM "********************************************\n"
#define TABLE_SIZE 256

#pragma region function prototypes
/* ============================================================================
 * Test function prototpes
 * ============================================================================
 */
int test_linalg_create_bind_matrix_00();
int test_linalg_create_bind_matrix_01a();
int test_linalg_create_bind_matrix_01b();
int test_linalg_create_bind_matrix_02();
int test_linalg_create_bind_matrix_03();
int test_linalg_create_bind_matrix_04a();
int test_linalg_create_bind_matrix_04b();
int test_linalg_create_bind_matrix_05();

int test_linalg_create_bind_vector_00();
int test_linalg_create_bind_vector_01a();
int test_linalg_create_bind_vector_01b();
int test_linalg_create_bind_vector_02();
int test_linalg_create_bind_vector_03();
int test_linalg_create_bind_vector_04();

int test_linalg_create_bind_scalar_00();
int test_linalg_create_bind_scalar_01a();
int test_linalg_create_bind_scalar_01b();

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
    set_log_level(LOG_ERROR);

    assert(test_linalg_create_bind_matrix_00() == 0);
    assert(test_linalg_create_bind_matrix_01a() == 0);
    assert(test_linalg_create_bind_matrix_01b() == 0);
    assert(test_linalg_create_bind_matrix_02() == 0);
    assert(test_linalg_create_bind_matrix_03() == 0);
    assert(test_linalg_create_bind_matrix_04a() == 0);
    assert(test_linalg_create_bind_matrix_04b() == 0);
    assert(test_linalg_create_bind_matrix_05() == 0);

    assert(test_linalg_create_bind_vector_00() == 0);
    assert(test_linalg_create_bind_vector_01a() == 0);
    assert(test_linalg_create_bind_vector_01b() == 0);
    assert(test_linalg_create_bind_vector_02() == 0);
    assert(test_linalg_create_bind_vector_03() == 0);
    assert(test_linalg_create_bind_vector_04() == 0);

    assert(test_linalg_create_bind_scalar_00() == 0);
    assert(test_linalg_create_bind_scalar_01a() == 0);
    assert(test_linalg_create_bind_scalar_01b() == 0);

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

int test_linalg_create_bind_matrix_01a()
{
    // Violates condition:    1. name != NULL.

    const char* test_name = "test_linalg_create_bind_matrix_01a";

    // create objects for test
    struct List elements = {0};
    size_t num_rows = 0;
    size_t num_cols = 0;
    return_valid_matrix_components(&elements, &num_rows, &num_cols);

    int rc = 1;

    // invalidate name
    const char* invalid_name = NULL;

    do
    {
        // init reg_table
        bool init_talbe_OK = (linalg_init_reg_table(TABLE_SIZE) == 0);
        if (init_talbe_OK == false)
        {
            printf("%s FAILED on init_table_OK.\n%s\n", test_name, DELIM);
            break;
        }

        bool null_name_rtns_3 =
            (linalg_create_bind_matrix(elements, num_rows, num_cols, invalid_name) == 3);
        if (null_name_rtns_3 == false)
        {
            printf("%s, FAILED on null_name_rtns_3.\n%s\n", test_name, DELIM);
            break;
        }

        printf("%s PASSED.\n%s\n", test_name, DELIM);
        rc = 0;

    } while (0);

    linalg_shutdown();
    return rc;
}

int test_linalg_create_bind_matrix_01b()
{
    // Violates condition:    1. name[0] != '\0'.

    const char* test_name = "test_linalg_create_bind_matrix_01b";

    // create objects for test
    struct List elements = {0};
    size_t num_rows = 0;
    size_t num_cols = 0;
    return_valid_matrix_components(&elements, &num_rows, &num_cols);

    int rc = 1;

    // invalidate name
    const char* invalid_name = "\0";

    do
    {
        // init reg_table
        bool init_table_OK = (linalg_init_reg_table(TABLE_SIZE) == 0);
        if (init_table_OK == false)
        {
            printf("%s FAILED on init_table_OK.\n%s\n", test_name, DELIM);
            break;
        }

        bool null_name_rtns_3 =
            (linalg_create_bind_matrix(elements, num_rows, num_cols, invalid_name) == 3);
        if (null_name_rtns_3 == false)
        {
            printf("%s, FAILED on null_name_rtns_3.\n%s\n", test_name, DELIM);
            break;
        }

        printf("%s PASSED.\n%s\n", test_name, DELIM);
        rc = 0;

    } while (0);

    linalg_shutdown();
    return rc;
}

int test_linalg_create_bind_matrix_02()
{
    // Violates condition:    2. elements.list != NULL.

    const char* test_name = "test_linalg_create_bind_matrix_02";

    // create objects for test
    struct List elements = {0};
    size_t num_rows = 0;
    size_t num_cols = 0;
    return_valid_matrix_components(&elements, &num_rows, &num_cols);
    const char* name = "test";

    int rc = 1;

    // invalidate elements.list
    free(elements.list);
    elements.list = NULL;

    do
    {
        // init reg_table
        bool init_table_OK = (linalg_init_reg_table(TABLE_SIZE) == 0);
        if (init_table_OK == false)
        {
            printf("%s FAILED on init_table_OK.\n%s\n", test_name, DELIM);
            break;
        }

        bool null_list_rtns_2 =
            (linalg_create_bind_matrix(elements, num_rows, num_cols, name) == 2);
        if (null_list_rtns_2 == false)
        {
            printf("%s FAILED on null_list_rtns_2.\n%s\n", test_name, DELIM);
            break;
        }

        printf("%s PASSED.\n%s\n", test_name, DELIM);
        rc = 0;

    } while (0);

    linalg_shutdown();
    return rc;
}

int test_linalg_create_bind_matrix_03()
{
    // Violates condition:    3. elements.type_size > 0.

    const char* test_name = "test_linalg_create_bind_matrix_03";

    // create objects for test
    struct List elements = {0};
    size_t num_rows = 0;
    size_t num_cols = 0;
    return_valid_matrix_components(&elements, &num_rows, &num_cols);
    const char* name = "test";

    int rc = 1;

    // invalidate elements.type_size
    elements.type_size = 0;

    do
    {
        // init reg_table
        bool init_table_OK = (linalg_init_reg_table(TABLE_SIZE) == 0);
        if (init_table_OK == false)
        {
            printf("%s FAILED on init_table_OK.\n%s\n", test_name, DELIM);
            break;
        }

        bool invalid_type_size_rtns_2 =
            (linalg_create_bind_matrix(elements, num_rows, num_cols, name) == 2);
        if (invalid_type_size_rtns_2 == false)
        {
            printf("%s FAILED on invalid_type_size_rtns_2.\n%s\n", test_name, DELIM);
            break;
        }

        printf("%s PASSED.\n%s\n", test_name, DELIM);
        rc = 0;

    } while (0);

    linalg_shutdown();
    return rc;
}

int test_linalg_create_bind_matrix_04a()
{
    // Violates condition:    4. num_rows > 0.

    const char* test_name = "test_linalg_create_bind_matrix_04a";

    // create objects for test
    struct List elements = {0};
    size_t num_rows = 0;
    size_t num_cols = 0;
    return_valid_matrix_components(&elements, &num_rows, &num_cols);
    const char* name = "test";

    int rc = 1;

    // invalidate num_rows
    num_rows = 0;

    do
    {
        // init reg_table
        bool init_table_OK = (linalg_init_reg_table(TABLE_SIZE) == 0);
        if (init_table_OK == false)
        {
            printf("%s FAILED on init_table_OK.\n%s\n", test_name, DELIM);
            break;
        }

        bool invalid_num_rows_rtns_2 =
            (linalg_create_bind_matrix(elements, num_rows, num_cols, name) == 2);
        if (invalid_num_rows_rtns_2 == false)
        {
            printf("%s FAILED on invalid_num_rows_rtns_2.\n%s\n", test_name, DELIM);
            break;
        }

        printf("%s PASSED.\n%s\n", test_name, DELIM);
        rc = 0;

    } while (0);

    linalg_shutdown();
    return rc;
}

int test_linalg_create_bind_matrix_04b()
{
    // Violates condition:    4. num_cols > 0.

    const char* test_name = "test_linalg_create_bind_matrix_04b";

    // create objects for test
    struct List elements = {0};
    size_t num_rows = 0;
    size_t num_cols = 0;
    return_valid_matrix_components(&elements, &num_rows, &num_cols);
    const char* name = "test";

    int rc = 1;

    // invalidate num_cols
    num_cols = 0;

    do
    {
        // init reg_table
        bool init_table_OK = (linalg_init_reg_table(TABLE_SIZE) == 0);
        if (init_table_OK == false)
        {
            printf("%s FAILED on init_table_OK.\n%s\n", test_name, DELIM);
            break;
        }

        bool invalid_num_cols_rtns_2 =
            (linalg_create_bind_matrix(elements, num_rows, num_cols, name) == 2);
        if (invalid_num_cols_rtns_2 == false)
        {
            printf("%s FAILED on invalid_num_rows_rtns_2.\n%s\n", test_name, DELIM);
            break;
        }

        printf("%s PASSED.\n%s\n", test_name, DELIM);
        rc = 0;

    } while (0);

    linalg_shutdown();
    return rc;
}

int test_linalg_create_bind_matrix_05()
{
    // Violates condition:    5. elements.size == num_rows * num_cols.

    const char* test_name = "test_linalg_create_bind_matrix_05";

    // create objects for test
    struct List elements = {0};
    size_t num_rows = 0;
    size_t num_cols = 0;
    return_valid_matrix_components(&elements, &num_rows, &num_cols);
    const char* name = "test";

    int rc = 1;

    // invalidate elements.size
    elements.size = 0;

    do
    {
        // init reg_table
        bool init_table_OK = (linalg_init_reg_table(TABLE_SIZE) == 0);
        if (init_table_OK == false)
        {
            printf("%s FAILED on init_table_OK.\n%s\n", test_name, DELIM);
            break;
        }

        bool invalid_size_rtns_2 =
            (linalg_create_bind_matrix(elements, num_rows, num_cols, name) == 2);
        if (invalid_size_rtns_2 == false)
        {
            printf("%s FAILED on invalid_size_rtns_2.\n%s\n", test_name, DELIM);
            break;
        }

        printf("%s PASSED.\n%s\n", test_name, DELIM);
        rc = 0;

    } while (0);

    linalg_shutdown();
    return rc;
}
#pragma endregion

#pragma region linalg_create_bind_vector() tests
int test_linalg_create_bind_vector_00()
{
    // test for valid input

    const char* test_name = "test_linalg_create_bind_vector_00";

    // create objects for test
    struct List elements = {0};
    return_valid_vector_components(&elements);
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

        bool rtn_0_OK = (linalg_create_bind_vector(elements, name) == 0);
        if (rtn_0_OK == false)
        {
            printf("%s FAILED on rtn_0_OK.\n%s\n", test_name, DELIM);
            break;
        }

        printf("%s PASSED.\n%s\n", test_name, DELIM);
        rc = 0;

    } while (0);

    linalg_shutdown();
    return rc;
}

int test_linalg_create_bind_vector_01a()
{
    // Violates condition:  1. name != NULL.

    const char* test_name = "test_linalg_create_bind_vector_01a";

    // create objects for test
    struct List elements = {0};
    return_valid_vector_components(&elements);

    int rc = 1;

    // invalidate name
    const char* name = NULL;

    do
    {
        // init reg_table
        bool init_table_OK = (linalg_init_reg_table(TABLE_SIZE) == 0);
        if (init_table_OK == false)
        {
            printf("%s FAILED on init_table_OK.\n%s\n", test_name, DELIM);
            break;
        }

        bool null_name_rtns_3 = (linalg_create_bind_vector(elements, name) == 3);
        if (null_name_rtns_3 == false)
        {
            printf("%s FAILED on null_name_rtns_3.\n%s\n", test_name, DELIM);
            break;
        }

        printf("%s PASSED.\n%s\n", test_name, DELIM);
        rc = 0;

    } while (0);

    linalg_shutdown();
    return rc;
}

int test_linalg_create_bind_vector_01b()
{
    // Violates condition:  2. name[0] != '\0'.

    const char* test_name = "test_linalg_create_bind_vector_01b";

    // create objects for test
    struct List elements = {0};
    return_valid_vector_components(&elements);

    int rc = 1;

    // invalidate name
    const char* name = "\0";

    do
    {
        // init reg_table
        bool init_table_OK = (linalg_init_reg_table(TABLE_SIZE) == 0);
        if (init_table_OK == false)
        {
            printf("%s FAILED on init_table_OK.\n%s\n", test_name, DELIM);
            break;
        }

        bool empty_name_rtns_3 = (linalg_create_bind_vector(elements, name) == 3);
        if (empty_name_rtns_3 == false)
        {
            printf("%s FAILED on empty_name_rtns_3.\n%s\n", test_name, DELIM);
            break;
        }

        printf("%s PASSED.\n%s\n", test_name, DELIM);
        rc = 0;

    } while (0);

    linalg_shutdown();
    return rc;
}

int test_linalg_create_bind_vector_02()
{
    // Violates condition:  2. elements.list != NULL.

    const char* test_name = "test_linalg_create_bind_vector_02";

    // create objects for test
    struct List elements = {0};
    return_valid_vector_components(&elements);
    const char* name = "test";

    int rc = 1;

    // invalidate elements.list
    free(elements.list);
    elements.list = NULL;

    do
    {
        // init reg_table
        bool init_table_OK = (linalg_init_reg_table(TABLE_SIZE) == 0);
        if (init_table_OK == false)
        {
            printf("%s FAILED on init_table_OK.\n%s\n", test_name, DELIM);
            break;
        }

        bool null_list_rtns_2 = (linalg_create_bind_vector(elements, name) == 2);
        if (null_list_rtns_2 == false)
        {
            printf("%s FAILED on null_list_rtns_2.\n%s\n", test_name, DELIM);
            break;
        }

        printf("%s PASSED.\n%s\n", test_name, DELIM);
        rc = 0;

    } while (0);

    linalg_shutdown();
    return rc;
}

int test_linalg_create_bind_vector_03()
{
    // Violates condition:  3. elements.size > 0.

    const char* test_name = "test_linalg_create_bind_vector_03";

    // create objects for test
    struct List elements = {0};
    return_valid_vector_components(&elements);
    const char* name = "test";

    int rc = 1;

    // invalidate elements.size
    elements.size = 0;

    do
    {
        // init reg_table
        bool init_table_OK = (linalg_init_reg_table(TABLE_SIZE) == 0);
        if (init_table_OK == false)
        {
            printf("%s FAILED on init_table_OK.\n%s\n", test_name, DELIM);
            break;
        }

        bool invalid_size_rtns_2 = (linalg_create_bind_vector(elements, name) == 2);
        if (invalid_size_rtns_2 == false)
        {
            printf("%s FAILED on invalid_size_rtns_2.\n%s\n", test_name, DELIM);
            break;
        }

        printf("%s PASSED.\n%s\n", test_name, DELIM);
        rc = 0;

    } while (0);

    linalg_shutdown();
    return rc;
}

int test_linalg_create_bind_vector_04()
{
    // Violates condition:  4. elements.type_size > 0.

    const char* test_name = "test_linalg_create_bind_vector_04";

    // create objects for test
    struct List elements = {0};
    return_valid_vector_components(&elements);
    const char* name = "test";

    int rc = 1;

    // invalidate elements.type_size
    elements.type_size = 0;

    do
    {
        // init reg_table
        bool init_table_OK = (linalg_init_reg_table(TABLE_SIZE) == 0);
        if (init_table_OK == false)
        {
            printf("%s FAILED on init_table_OK.\n%s\n", test_name, DELIM);
            break;
        }

        bool invalid_type_size_rtns_2 = (linalg_create_bind_vector(elements, name) == 2);
        if (invalid_type_size_rtns_2 == false)
        {
            printf("%s FAILED on invalid_type_size_rtns_2.\n%s\n", test_name, DELIM);
            break;
        }

        printf("%s PASSED.\n%s\n", test_name, DELIM);
        rc = 0;

    } while (0);

    linalg_shutdown();
    return rc;
}
#pragma endregion

#pragma region linalg_create_bind_scalar() tests
int test_linalg_create_bind_scalar_00()
{
    // tests valid input

    const char* test_name = "test_linalg_create_bind_scalar_00";

    // create objects for test
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

        bool rtn_0_OK = (linalg_create_bind_scalar(3.14, name) == 0);
        if (rtn_0_OK == false)
        {
            printf("%s FAILED on rtn_0_OK.\n%s\n", test_name, DELIM);
            break;
        }

        printf("%s PASSED.\n%s\n", test_name, DELIM);
        rc = 0;

    } while (0);

    linalg_shutdown();
    return rc;
}

int test_linalg_create_bind_scalar_01a()
{
    // Violates condition:  1. name != NULL.

    const char* test_name = "test_linalg_create_bind_scalar_01a";

    // create objects for test
    const char* name = NULL;

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

        bool null_name_rtns_3 = (linalg_create_bind_scalar(3.14, name) == 3);
        if (null_name_rtns_3 == false)
        {
            printf("%s FAILED on null_name_rtns_3.\n%s\n", test_name, DELIM);
            break;
        }

        printf("%s PASSED.\n%s\n", test_name, DELIM);
        rc = 0;

    } while (0);

    linalg_shutdown();
    return rc;
}

int test_linalg_create_bind_scalar_01b()
{
    // Violates condition:  1. name[0] != '\0'.

    const char* test_name = "test_linalg_create_bind_scalar_01b";

    // create objects for test
    const char* name = "\0";

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

        bool empty_name_rtns_3 = (linalg_create_bind_scalar(3.14, name) == 3);
        if (empty_name_rtns_3 == false)
        {
            printf("%s FAILED on empty_name_rtns_3.\n%s\n", test_name, DELIM);
            break;
        }

        printf("%s PASSED.\n%s\n", test_name, DELIM);
        rc = 0;

    } while (0);

    linalg_shutdown();
    return rc;
}
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