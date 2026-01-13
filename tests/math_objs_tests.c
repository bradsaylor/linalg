#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/linalg.h"
#include "../include/math_objs.h"

#define DELIM "********************************************\n"

#pragma region function prototypes
/* ============================================================================
 * Test function prototpes
 * ============================================================================
 */
int test_create_matrix_00();
int test_create_matrix_01();
int test_create_matrix_02();
int test_create_matrix_03();
int test_create_matrix_04();
int test_create_matrix_05();

int test_create_vector_00();
int test_create_vector_01();
int test_create_vector_02();
int test_create_vector_03();

int test_decref_obj_00();
int test_decref_obj_01();

int test_incref_obj_00();
int test_incref_obj_01();

/* ============================================================================
 * Helper function prototypes
 * ============================================================================
 */
int return_valid_matrix_components(struct List *elements, size_t *num_rows,
                                   size_t *num_cols);
int return_valid_vector_components(struct List *elements);
#pragma endregion

#pragma region main()
/* ============================================================================
 * main()
 * ============================================================================
 */
int main()
{
    assert(test_create_matrix_00() == 0);
    assert(test_create_matrix_01() == 0);
    assert(test_create_matrix_02() == 0);
    assert(test_create_matrix_03() == 0);
    assert(test_create_matrix_04() == 0);
    assert(test_create_matrix_05() == 0);

    assert(test_create_vector_00() == 0);
    assert(test_create_vector_01() == 0);
    assert(test_create_vector_02() == 0);
    assert(test_create_vector_03() == 0);

    assert(test_decref_obj_00() == 0);
    assert(test_decref_obj_01() == 0);

    assert(test_incref_obj_00() == 0);
    assert(test_incref_obj_01() == 0);

    return 0;
}
#pragma endregion

#pragma region create_matrix() tests
/* ============================================================================
 * create_matrix() tests
 * ============================================================================
 */
int test_create_matrix_00()
{
    // Test case for valid input
    // Also verifies assigned OBJ_TYPE and decref_obj() functionality

    const char *test_name = "test_create_matrix_00";

    // validation flags
    bool rtn_comps_OK = false;
    bool rtn_wrapper_OK = false;
    bool obj_type_OK = false;
    bool decref_rtn_OK = false;

    // components for matrix construction
    struct List elements = {0};
    size_t num_rows = 0;
    size_t num_cols = 0;

    struct ObjWrapper *new_matrix = NULL;

    // get matrix components
    int rtn_comps =
        return_valid_matrix_components(&elements, &num_rows, &num_cols);

    rtn_comps_OK = (rtn_comps == 0);
    if (rtn_comps_OK == false)
    { // if failed to get components
        printf("%s FAILED on rtn_comps_OK.\n%s\n", test_name, DELIM);
        return 1;
    }

    new_matrix = create_matrix(elements, num_rows, num_cols);
    rtn_wrapper_OK = (new_matrix != NULL);

    if (rtn_wrapper_OK == false)
    {                        // if failed create_matrix()
        free(elements.list); // must free List.list ourselves
        printf("%s FAILED on rtn_wrapper_OK.\n%s\n", test_name, DELIM);
        return 1;
    }

    obj_type_OK = (get_obj_type(new_matrix) == OBJ_MATRIX);
    if (obj_type_OK == false)
        printf("%s FAILED on obj_type_OK.\n%s\n", test_name, DELIM);

    // decref_obj() to safely free wrapper (including List.list)
    decref_rtn_OK = (decref_obj(new_matrix) == 0);
    if (decref_rtn_OK == false)
        printf("%s FAILED on decref_rtn_OK.\n%s\n", test_name, DELIM);

    if ((obj_type_OK == false) || (decref_rtn_OK == false))
        return 1;

    printf("%s PASSED.\n%s\n", test_name, DELIM);
    return 0;
}

int test_create_matrix_01()
{
    //  Violates condition:   1.  elements.list != NULL.

    const char *test_name = "test_create_matrix_01";

    // validation flags
    bool rtn_comps_OK = false;
    bool returns_NULL = false;

    // components for matrix construction
    struct List elements = {0};
    size_t num_rows = 0;
    size_t num_cols = 0;

    struct ObjWrapper *new_matrix = NULL;

    // get matrix components
    int rtn_comps =
        return_valid_matrix_components(&elements, &num_rows, &num_cols);

    rtn_comps_OK = (rtn_comps == 0);
    if (rtn_comps_OK == false)
    { // if failed to get components
        printf("%s FAILED on rtn_comps_OK.\n%s\n", test_name, DELIM);
        return 1;
    }

    // Store allocated List.list for freeing on failure
    double *old_list = elements.list;
    // Replace valid List.list with invalid one
    elements.list = NULL;

    new_matrix = create_matrix(elements, num_rows, num_cols);
    returns_NULL = (new_matrix == NULL);

    free(old_list);

    if (returns_NULL == false)
    {                           // if return != NULL
        decref_obj(new_matrix); // must free wrapper
        printf("%s FAILED on returns_NULL.\n%s\n", test_name, DELIM);
        return 1;
    }

    printf("%s PASSED.\n%s\n", test_name, DELIM);
    return 0;
}

int test_create_matrix_02()
{
    //  Violates condition:   2.  elements.type_size > 0.

    const char *test_name = "test_create_matrix_02";

    // validation flags
    bool rtn_comps_OK = false;
    bool returns_NULL = false;

    // components for matrix construction
    struct List elements = {0};
    size_t num_rows = 0;
    size_t num_cols = 0;

    struct ObjWrapper *new_matrix = NULL;

    // get matrix components
    int rtn_comps =
        return_valid_matrix_components(&elements, &num_rows, &num_cols);

    rtn_comps_OK = (rtn_comps == 0);
    if (rtn_comps_OK == false)
    { // if failed to get components
        printf("%s FAILED on rtn_comps_OK.\n%s\n", test_name, DELIM);
        return 1;
    }

    // Invalidate List.type_size
    elements.type_size = 0;

    new_matrix = create_matrix(elements, num_rows, num_cols);
    returns_NULL = (new_matrix == NULL);

    if (returns_NULL == false)
    {                           // if return != NULL
        decref_obj(new_matrix); // must free wrapper
        printf("%s FAILED on returns_NULL.\n%s\n", test_name, DELIM);
        return 1;
    }

    free(elements.list); // must free List.list ourselves
    printf("%s PASSED.\n%s\n", test_name, DELIM);
    return 0;
}

int test_create_matrix_03()
{
    //  Violates condition:   3.  num_rows > 0.

    const char *test_name = "test_create_matrix_03";

    // validation flags
    bool rtn_comps_OK = false;
    bool returns_NULL = false;

    // components for matrix construction
    struct List elements = {0};
    size_t num_rows = 0;
    size_t num_cols = 0;

    struct ObjWrapper *new_matrix = NULL;

    // get matrix components
    int rtn_comps =
        return_valid_matrix_components(&elements, &num_rows, &num_cols);

    rtn_comps_OK = (rtn_comps == 0);
    if (rtn_comps_OK == false)
    { // if failed to get components
        printf("%s FAILED on rtn_comps_OK.\n%s\n", test_name, DELIM);
        return 1;
    }

    // Invalidate num_rows
    num_rows = 0;

    new_matrix = create_matrix(elements, num_rows, num_cols);
    returns_NULL = (new_matrix == NULL);

    if (returns_NULL == false)
    {                           // if return != NULL
        decref_obj(new_matrix); // must free wrapper
        printf("%s FAILED on returns_NULL.\n%s\n", test_name, DELIM);
        return 1;
    }

    free(elements.list); // must free List.list ourselves
    printf("%s PASSED.\n%s\n", test_name, DELIM);
    return 0;
}

int test_create_matrix_04()
{
    //  Violates condition:   4.  num_cols > 0.

    const char *test_name = "test_create_matrix_04";

    // validation flags
    bool rtn_comps_OK = false;
    bool returns_NULL = false;

    // components for matrix construction
    struct List elements = {0};
    size_t num_rows = 0;
    size_t num_cols = 0;

    struct ObjWrapper *new_matrix = NULL;

    // get matrix components
    int rtn_comps =
        return_valid_matrix_components(&elements, &num_rows, &num_cols);

    rtn_comps_OK = (rtn_comps == 0);
    if (rtn_comps_OK == false)
    { // if failed to get components
        printf("%s FAILED on rtn_comps_OK.\n%s\n", test_name, DELIM);
        return 1;
    }

    // Invalidate num_cols
    num_cols = 0;

    new_matrix = create_matrix(elements, num_rows, num_cols);
    returns_NULL = (new_matrix == NULL);

    if (returns_NULL == false)
    {                           // if return != NULL
        decref_obj(new_matrix); // must free wrapper
        printf("%s FAILED on returns_NULL.\n%s\n", test_name, DELIM);
        return 1;
    }

    free(elements.list); // must free List.list ourselves
    printf("%s PASSED.\n%s\n", test_name, DELIM);
    return 0;
}

int test_create_matrix_05()
{
    //  Violates condition:   5.  elements.size == num_rows * num_cols.

    const char *test_name = "test_create_matrix_05";

    // validation flags
    bool rtn_comps_OK = false;
    bool returns_NULL = false;

    // components for matrix construction
    struct List elements = {0};
    size_t num_rows = 0;
    size_t num_cols = 0;

    struct ObjWrapper *new_matrix = NULL;

    // get matrix components
    int rtn_comps =
        return_valid_matrix_components(&elements, &num_rows, &num_cols);

    rtn_comps_OK = (rtn_comps == 0);
    if (rtn_comps_OK == false)
    { // if failed to get components
        printf("%s FAILED on rtn_comps_OK.\n%s\n", test_name, DELIM);
        return 1;
    }

    // Invalidate elements.size
    elements.size = ++elements.size;

    new_matrix = create_matrix(elements, num_rows, num_cols);
    returns_NULL = (new_matrix == NULL);

    if (returns_NULL == false)
    {                           // if return != NULL
        decref_obj(new_matrix); // must free wrapper
        printf("%s FAILED on returns_NULL.\n%s\n", test_name, DELIM);
        return 1;
    }

    free(elements.list); // must free List.list ourselves
    printf("%s PASSED.\n%s\n", test_name, DELIM);
    return 0;
}
#pragma endregion

#pragma region create_vector() tests
/* ============================================================================
 * create_vector() tests
 * ============================================================================
 */
int test_create_vector_00()
{
    // Test case for valid input
    // Also verifies assigned OBJ_TYPE and decref_obj() functionality

    const char *test_name = "test_create_vector_00";

    // validation flags
    bool rtn_comps_OK = false;
    bool rtn_wrapper_OK = false;
    bool obj_type_OK = false;
    bool decref_rtn_OK = false;

    // components for vector construction
    struct List elements = {0};

    struct ObjWrapper *new_vector = NULL;

    // get vector list
    int rtn_comps = return_valid_vector_components(&elements);

    rtn_comps_OK = (rtn_comps == 0);
    if (rtn_comps_OK == false)
    { // if failed to get components
        printf("%s FAILED on rtn_comps_OK.\n%s\n", test_name, DELIM);
        return 1;
    }

    new_vector = create_vector(elements);
    rtn_wrapper_OK = (new_vector != NULL);

    if (rtn_wrapper_OK == false)
    {                        // if failed create_vector()
        free(elements.list); // must free List.list ourselves
        printf("%s FAILED on rtn_wrapper_OK.\n%s\n", test_name, DELIM);
        return 1;
    }

    obj_type_OK = (get_obj_type(new_vector) == OBJ_VECTOR);
    if (obj_type_OK == false)
        printf("%s FAILED on obj_type_OK.\n%s\n", test_name, DELIM);

    // decref_obj() to safely free wrapper (including List.list)
    decref_rtn_OK = (decref_obj(new_vector) == 0);
    if (decref_rtn_OK == false)
        printf("%s FAILED on decref_rtn_OK.\n%s\n", test_name, DELIM);

    if ((obj_type_OK == false) || (decref_rtn_OK == false))
        return 1;

    printf("%s PASSED.\n%s\n", test_name, DELIM);
    return 0;
}

int test_create_vector_01()
{
    //  Violates condition:   1.  elements.list != NULL.

    const char *test_name = "test_create_vector_01";

    // validation flags
    bool rtn_comps_OK = false;
    bool returns_NULL = false;

    // components for vector construction
    struct List elements = {0};

    struct ObjWrapper *new_vector = NULL;

    // get vector list
    int rtn_comps = return_valid_vector_components(&elements);

    rtn_comps_OK = (rtn_comps == 0);
    if (rtn_comps_OK == false)
    { // if failed to get components
        printf("%s FAILED on rtn_comps_OK.\n%s\n", test_name, DELIM);
        return 1;
    }

    // Store allocated List.list for freeing on failure
    double *old_list = elements.list;
    // Replace valid List.list with invalid one
    elements.list = NULL;

    new_vector = create_vector(elements);
    returns_NULL = (new_vector == NULL);

    free(old_list);

    if (returns_NULL == false)
    {                           // if return != NULL
        decref_obj(new_vector); // must free wrapper
        printf("%s FAILED on returns_NULL.\n%s\n", test_name, DELIM);
        return 1;
    }

    printf("%s PASSED.\n%s\n", test_name, DELIM);
    return 0;
}

int test_create_vector_02()
{
    //  Violates condition:   2.  elements.type_size > 0.

    const char *test_name = "test_create_vector_02";

    // validation flags
    bool rtn_comps_OK = false;
    bool returns_NULL = false;

    // components for vector construction
    struct List elements = {0};

    struct ObjWrapper *new_vector = NULL;

    // get vector list
    int rtn_comps = return_valid_vector_components(&elements);

    rtn_comps_OK = (rtn_comps == 0);
    if (rtn_comps_OK == false)
    { // if failed to get components
        printf("%s FAILED on rtn_comps_OK.\n%s\n", test_name, DELIM);
        return 1;
    }

    // Invalidate List.type_size
    elements.type_size = 0;

    new_vector = create_vector(elements);
    returns_NULL = (new_vector == NULL);

    if (returns_NULL == false)
    {                           // if return != NULL
        decref_obj(new_vector); // must free wrapper
        printf("%s FAILED on returns_NULL.\n%s\n", test_name, DELIM);
        return 1;
    }

    free(elements.list); // must free List.list ourselves
    printf("%s PASSED.\n%s\n", test_name, DELIM);
    return 0;
}

int test_create_vector_03()
{
    //  Violates condition:   3.  elements.size > 0.

    const char *test_name = "test_create_vector_03";

    // validation flags
    bool rtn_comps_OK = false;
    bool returns_NULL = false;

    // components for vector construction
    struct List elements = {0};

    struct ObjWrapper *new_vector = NULL;

    // get vector list
    int rtn_comps = return_valid_vector_components(&elements);

    rtn_comps_OK = (rtn_comps == 0);
    if (rtn_comps_OK == false)
    { // if failed to get components
        printf("%s FAILED on rtn_comps_OK.\n%s\n", test_name, DELIM);
        return 1;
    }

    // Invalidate List.size
    elements.size = 0;

    new_vector = create_vector(elements);
    returns_NULL = (new_vector == NULL);

    if (returns_NULL == false)
    {                           // if return != NULL
        decref_obj(new_vector); // must free wrapper
        printf("%s FAILED on returns_NULL.\n%s\n", test_name, DELIM);
        return 1;
    }

    free(elements.list); // must free List.list ourselves
    printf("%s PASSED.\n%s\n", test_name, DELIM);
    return 0;
}
#pragma endregion

#pragma region create_scalar() tests
/* ============================================================================
 * create_scalar() tests
 * ============================================================================
 */

// None.

#pragma endregion

#pragma region decref_obj() tests
/* ============================================================================
 * decref_obj() tests
 * ============================================================================
 */
int test_decref_obj_00()
{
    // test case for valid input

    const char *test_name = "test_decref_obj_00";

    bool create_obj_OK = false;
    bool decref_obj_OK = false;

    // create ObjWrapper
    struct ObjWrapper *new_scalar = create_scalar(3.14);

    create_obj_OK = (new_scalar != NULL);
    if (create_obj_OK == false)
    {
        printf("%s FAILED on create_obj_OK.\n%s\n", test_name, DELIM);
        return 1;
    }

    decref_obj_OK = (decref_obj(new_scalar) == 0);
    if (decref_obj_OK == false)
    {
        printf("%s FAILED on decref_obj_OK.\n%s\n", test_name, DELIM);
        return 1;
    }

    printf("%s PASSED.\n%s\n", test_name, DELIM);
    return 0;
}

int test_decref_obj_01()
{
    //  Violates condition:   1.  wrapper != NULL.

    const char *test_name = "test_decref_obj_01";

    bool decref_obj_rtn_1 = false;

    struct ObjWrapper *null_wrapper = NULL;

    decref_obj_rtn_1 = (decref_obj(null_wrapper) == 1);
    if (decref_obj_rtn_1 == false)
    {
        printf("%s FAILED on decref_obj_rtn_1.\n%s\n", test_name, DELIM);
        return 1;
    }

    printf("%s PASSED.\n%s\n", test_name, DELIM);
    return 0;
}
#pragma endregion

#pragma region incref_obj() tests
/* ============================================================================
 * incref_obj() tests
 * ============================================================================
 */
int test_incref_obj_00()
{
    // test case for valid input

    const char *test_name = "test_incref_obj_00";

    bool create_obj_OK = false;
    bool incref_obj_OK = false;

    // create ObjWrapper
    struct ObjWrapper *new_scalar = create_scalar(3.14);

    create_obj_OK = (new_scalar != NULL);
    if (create_obj_OK == false)
    {
        printf("%s FAILED on create_obj_OK.\n%s\n", test_name, DELIM);
        return 1;
    }

    incref_obj_OK = (incref_obj(new_scalar) == 0);
    if (incref_obj_OK == false)
    {
        printf("%s FAILED on incref_obj_OK.\n%s\n", test_name, DELIM);
        return 1;
    }

    decref_obj(new_scalar); // realease both wrapper refs on success
    decref_obj(new_scalar); // one for create and one for incref

    printf("%s PASSED.\n%s\n", test_name, DELIM);
    return 0;
}

int test_incref_obj_01()
{
    //  Violates condition:   1.  wrapper != NULL.

    const char *test_name = "test_incref_obj_01";

    bool incref_obj_rtn_1 = false;

    struct ObjWrapper *null_wrapper = NULL;

    incref_obj_rtn_1 = (incref_obj(null_wrapper) == 1);
    if (incref_obj_rtn_1 == false)
    {
        printf("%s FAILED on incref_obj_rtn_1.\n%s\n", test_name, DELIM);
        return 1;
    }

    printf("%s PASSED.\n%s\n", test_name, DELIM);
    return 0;
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
int return_valid_matrix_components(struct List *elements, size_t *num_rows,
                                   size_t *num_cols)
{
    size_t element_list_size = 8;
    size_t rows = 4;
    size_t cols = 2;

    double list_to_copy[8] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0};
    double *element_list = malloc(element_list_size * sizeof(double));
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
int return_valid_vector_components(struct List *elements)
{
    size_t element_list_size = 8;

    double list_to_copy[8] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0};
    double *element_list = malloc(element_list_size * sizeof(double));
    if (!element_list)
        return 2; // allocation failure

    memcpy(element_list, list_to_copy, sizeof(list_to_copy));

    elements->list = element_list;
    elements->size = element_list_size;
    elements->type_size = sizeof(double);

    return 0;
};
#pragma endregion