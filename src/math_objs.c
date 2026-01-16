#include "../include/math_objs.h"
#include "../include/logs.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

#pragma region Local Definitions
/* ============================================================================
 * File-local definitions
 * ============================================================================
 */

struct ObjWrapper
{
    void* obj;
    enum ObjType type;
    size_t ref_count;
};

struct Matrix
{
    struct List elements; // owns elements.list (heap)
    size_t num_rows;
    size_t num_cols;
};

struct Vector
{
    struct List elements;
};

struct Scalar
{
    double value;
};

struct ObjLLNode
{
    struct ObjWrapper* object;
    struct ObjLLNode* next;
};

struct ObjLL
{
    struct ObjLLNode* head;
    size_t count;
};

static struct ObjLL obj_list;
#pragma endregion

#pragma region Private Function Prototypes
/* ============================================================================
 * Private function prototypes
 * ============================================================================
 */
static int destroy_matrix(struct Matrix* matrix);
static int destroy_vector(struct Vector* vector);
static int destroy_scalar(struct Scalar* scalar);
static int destroy_wrapper(struct ObjWrapper* wrapper);
static int add_obj(struct ObjWrapper* object);
static int remove_obj(struct ObjWrapper* object);
static int destroy_obj(struct ObjWrapper* wrapper);
static struct ObjLLNode* find_node(struct ObjWrapper* wrapper, struct ObjLLNode* prev_node);
#pragma endregion

#pragma region Public API
/* ============================================================================
 * Public API implementation
 * ============================================================================
 */

// Pre conditions:
//   1.  elements.list != NULL.
//   2.  elements.type_size > 0.
//   3.  num_rows > 0.
//   4.  num_cols > 0.
//   5.  elements.size == num_rows * num_cols.
// Post conditions: None.
struct ObjWrapper* create_matrix(struct List elements, size_t num_rows, size_t num_cols)
{
    if (!elements.list)
        return NULL; // No matrix element list

    // Check for rows/cols/size consistency
    if ((!num_cols) || (!num_rows))
        return NULL; // rows and or cols == 0
    if ((num_rows * num_cols) != elements.size)
        return NULL; // size != rows*cols
    if (elements.type_size == 0)
        return NULL; // zero type size

    // Allocate matrix object and wrapper
    struct Matrix* new_matrix = malloc(sizeof(struct Matrix));
    if (!new_matrix)
    {
        LOG_OUT(LOG_ERROR, "Failed to malloc %zu bytes for new matrix (%zuX%zu).",
                sizeof(struct Matrix), num_rows, num_cols);
        return NULL;
    }

    struct ObjWrapper* new_wrapper = malloc(sizeof(struct ObjWrapper));
    if (!new_wrapper)
    {
        LOG_OUT(LOG_ERROR, "Failed to malloc %zu bytes for new wrapper (matrix %zuX%zu).",
                sizeof(struct ObjWrapper), num_rows, num_cols);
        free(new_matrix);
        return NULL;
    }

    // Populate matrix and wrapper
    new_matrix->num_rows = num_rows;
    new_matrix->num_cols = num_cols;

    new_wrapper->obj = new_matrix;
    new_wrapper->type = OBJ_MATRIX;
    new_wrapper->ref_count = 1;

    // Add wrapper to object list
    int add_obj_ret = add_obj(new_wrapper);
    if (add_obj_ret)
    {
        LOG_OUT(LOG_ERROR,
                "add_obj() failed: wrapper=%p obj=%p type=MATRIX dims=%zuX%zu ret=%d.",
                new_wrapper, new_wrapper->obj, num_rows, num_cols, add_obj_ret);
        free(new_matrix);
        free(new_wrapper);
        return NULL;
    }

    // Pass ownership of elements.list to new_matrix
    new_matrix->elements = elements;

    LOG_OUT(LOG_DEBUG, "succeeded: wrapper=%p obj=%p type=MATRIX dims=%zuX%zu.", new_wrapper,
            new_wrapper->obj, num_rows, num_cols);
    return new_wrapper;
}

//  Pre conditions:
//    1.  elements.list != NULL.
//    2.  elements.size > 0.
//    3.  elements.type_size > 0.
//  Post conditions: None.
struct ObjWrapper* create_vector(struct List elements)
{
    // Check elements to make sure we have list, size, and type_size
    if (!elements.list)
        return NULL; // No list
    if (elements.size == 0)
        return NULL; // zero size
    if (elements.type_size == 0)
        return NULL; // zero type size

    struct Vector* new_vector = malloc(sizeof(struct Vector));
    if (!new_vector)
    {
        LOG_OUT(LOG_ERROR, "Failed to malloc %zu bytes for new vector dim=%zu.",
                sizeof(struct Vector), elements.size);
        return NULL;
    }

    struct ObjWrapper* new_wrapper = malloc(sizeof(struct ObjWrapper));
    if (!new_wrapper)
    {
        LOG_OUT(LOG_ERROR, "Failed to malloc %zu bytes for new wrapper (vector dim=%zu).",
                sizeof(struct ObjWrapper), elements.size);
        free(new_vector);
        return NULL;
    }

    new_wrapper->obj = new_vector;
    new_wrapper->type = OBJ_VECTOR;
    new_wrapper->ref_count = 1;

    int add_obj_ret = add_obj(new_wrapper);
    if (add_obj_ret)
    { // failed add_obj()
        LOG_OUT(LOG_ERROR, "add_obj() failed: wrapper=%p obj=%p type=VECTOR dim=%zu ret=%d.",
                new_wrapper, new_wrapper->obj, elements.size, add_obj_ret);
        free(new_vector);
        free(new_wrapper);
        return NULL;
    }

    // Pass ownership of elements.list to new_vector
    new_vector->elements = elements;

    LOG_OUT(LOG_DEBUG, "succeeded: wrapper=%p obj=%p type=VECTOR dim=%zu.", new_wrapper,
            new_wrapper->obj, elements.size);
    return new_wrapper;
}

//  Pre conditions: None.
//  Post conditions: None.
struct ObjWrapper* create_scalar(double value)
{
    struct Scalar* new_scalar = malloc(sizeof(struct Scalar));
    if (!new_scalar)
    {
        LOG_OUT(LOG_ERROR, "Failed to malloc %zu bytes for new scalar.", sizeof(struct Scalar));
        return NULL;
    }

    struct ObjWrapper* new_wrapper = malloc(sizeof(struct ObjWrapper));
    if (!new_wrapper)
    {
        LOG_OUT(LOG_ERROR, "Failed to malloc %zu bytes for new wrapper (scalar).",
                sizeof(struct ObjWrapper));
        free(new_scalar);
        return NULL;
    }

    new_scalar->value = value;

    new_wrapper->obj = new_scalar;
    new_wrapper->type = OBJ_SCALAR;
    new_wrapper->ref_count = 1;

    int add_obj_ret = add_obj(new_wrapper);
    if (add_obj_ret)
    { // failed add_obj()
        LOG_OUT(LOG_ERROR, "add_obj() failed: wrapper=%p obj=%p type=SCALAR ret=%d.",
                new_wrapper, new_wrapper->obj, add_obj_ret);
        free(new_scalar);
        free(new_wrapper);
        return NULL;
    }

    LOG_OUT(LOG_DEBUG, "succeeded: wrapper=%p obj=%p type=SCALAR.", new_wrapper, new_wrapper->obj);
    return new_wrapper;
}

int destroy_obj(struct ObjWrapper* wrapper)
{
    if (!wrapper)
        return 0; // no wrapper is noop

    if (wrapper->type != OBJ_MATRIX && wrapper->type != OBJ_VECTOR && wrapper->type != OBJ_SCALAR)
    {
        LOG_OUT(LOG_ERROR, "wrapper=%p obj=%p has invalid type=%d.", wrapper, wrapper->obj,
                wrapper->type);
        return 4; // invalid type
    }

    int remove_ret = remove_obj(wrapper);
    if (remove_ret)
    {
        LOG_OUT(LOG_ERROR, "remove_obj() failed wrapper=%p obj=%p type=%d ret=%d.", wrapper,
                wrapper->obj, wrapper->type, remove_ret);
        return remove_ret; // remove failed
    }

    switch (wrapper->type)
    {
    case OBJ_MATRIX:
        destroy_matrix((struct Matrix*)wrapper->obj);
        break;
    case OBJ_VECTOR:
        destroy_vector((struct Vector*)wrapper->obj);
        break;
    case OBJ_SCALAR:
        destroy_scalar((struct Scalar*)wrapper->obj);
        break;
    default:
        LOG_OUT(LOG_ERROR, "invariant violated wrapper=%p obj=%p type=%d.", wrapper,
                wrapper->obj, wrapper->type);
        assert(false); // should be unreachable
        return 3;
    }

    wrapper->obj = NULL;
    destroy_wrapper(wrapper);
    return 0;
}

// pre conditions:
//  1.  wrapper != NULL.
// post conditions: None.
int incref_obj(struct ObjWrapper* wrapper)
{
    if (!wrapper)
        return 1; // caller error
    if (wrapper->ref_count == 0)
    {
        LOG_OUT(LOG_ERROR, "invariant violated: wrapper=%p obj=%p type=%d rc=%zu.", wrapper,
                wrapper->obj, wrapper->type, wrapper->ref_count);
        return 3; // internal error
    }

    size_t old_rc = wrapper->ref_count;
    wrapper->ref_count++;
    LOG_OUT(LOG_DEBUG, "succeeded wrapper=%p obj=%p type=%d rc:%zu->%zu.", wrapper, wrapper->obj,
            wrapper->type, old_rc, wrapper->ref_count);
    return 0;
}

//  Pre conditions:
//    1.  wrapper != NULL.
//  Post conditions: None.
int decref_obj(struct ObjWrapper* wrapper)
{
    if (!wrapper)
        return 1; // caller error

    assert(wrapper->ref_count != 0);
    if (wrapper->ref_count == 0)
    {
        LOG_OUT(LOG_ERROR, "invariant violated: wrapper=%p obj=%p type=%d rc=%zu.", wrapper,
                wrapper->obj, wrapper->type, wrapper->ref_count);
        return 3; // internal error
    }

    size_t old_rc = wrapper->ref_count;
    wrapper->ref_count--;
    if (wrapper->ref_count == 0) // destroy obj if ref_count -> 0
    {
        LOG_OUT(LOG_DEBUG, "wrapper=%p obj=%p type=%d rc:%zu->%zu (destroy).", wrapper,
                wrapper->obj, wrapper->type, old_rc, wrapper->ref_count);
        destroy_obj(wrapper);
        return 0;
    }

    LOG_OUT(LOG_DEBUG, "succeeded wrapper=%p obj=%p type=%d rc:%zu->%zu.", wrapper, wrapper->obj,
            wrapper->type, old_rc, wrapper->ref_count);
    return 0;
}

//  Pre conditions:
//    1.  wrapper != NULL.
//  Post conditions: None.
enum ObjType get_obj_type(const struct ObjWrapper* wrapper)
{
    if (!wrapper)
        return OBJ_NONE; // wrapper is NULL
    return wrapper->type;
}

//  Pre conditions: None.
//  Post conditions: None.
//  Asserts invariant: obj->ref_count == 1 for all objects before teardown.
//  Asserts invariant: obj_list.count == 0 after teardown.
//  Asserts invariant: obj_list.head == NULL after teardown.
int destroy_obj_list()
{
    while (obj_list.head)
    {
        assert(obj_list.head->object->ref_count == 1);

        int decref_ret = decref_obj(obj_list.head->object);
        assert(decref_ret == 0);
    }

    assert(obj_list.count == 0 && obj_list.head == NULL);

    return 0;
}

/* ============================================================================
 * Public debug functions
 * ============================================================================
 */
//  Pre conditions:
//   1. wrapper != NULL.
//  post conditions: None.
size_t debug_get_obj_refcount(const struct ObjWrapper* wrapper)
{
#ifdef NDEBUG
    return -1;
#else
    if (!wrapper)
        return -1; // invalid input
    return wrapper->ref_count;
#endif
}
#pragma endregion

#pragma region Private Functions
/* ============================================================================
 * Private helper implementation
 * ============================================================================
 */

//  Purpose: Destroy matrix struct and allocated members.
//  Input Assumptions: None.
//  Effects: `matrix` and `matrix->elements.list` freed.
//  Returns: 0 in all cases.
//  Notes: None.
static int destroy_matrix(struct Matrix* matrix)
{
    if (!matrix)
        return 0;
    free(matrix->elements.list);
    free(matrix);
    return 0;
}

//  Purpose: Destroy vector struct and allocated members.
//  Input Assumptions: None.
//  Effects: `vector` and `vector->elements.list` freed.
//  Returns: 0 in all cases.
//  Notes: None.
static int destroy_vector(struct Vector* vector)
{
    if (!vector)
        return 0;
    free(vector->elements.list);
    free(vector);
    return 0;
}

//  Purpose: Destroy scalar struct.
//  Input Assumptions: None.
//  Effects: `scalar` freed.
//  Returns: 0 in all cases.
//  Notes: None.
static int destroy_scalar(struct Scalar* scalar)
{
    if (!scalar)
        return 0;
    free(scalar);
    return 0;
}

//  Purpose: Destroy wrapper struct.
//  Input Assumptions: None.
//  Effects: `wrapper` freed.
//  Returns: 0 in all cases.
//  Notes: None.
static int destroy_wrapper(struct ObjWrapper* wrapper)
{
    if (!wrapper)
        return 0;
    free(wrapper);
    return 0;
}

//  Purpose: Add new object to the object linked list.
//  Input Assumptions: None.
//  Effects: `object` added to object list and list parameters updated.
//  Returns:
//    0: Success.
//    1: Invalid input.
//    2: Allocation error.
//    3: Internal invariance violation.
//  Notes: Enforces invariant: one `obj_list` reference per object.
static int add_obj(struct ObjWrapper* object)
{
    // return immediately for invalid input
    if (!object)
        return 1; // caller error

    // check if object is already in the object list
    struct ObjLLNode* prev_node = NULL;
    struct ObjLLNode* found_node = find_node(object, prev_node);
    if (found_node)
        return 3; // invariant violation

    // create new object list node
    struct ObjLLNode* new_node = malloc(sizeof(struct ObjLLNode));
    if (!new_node)
        return 2; // allocation error

    // populate new object list node
    new_node->next = obj_list.head;
    new_node->object = object;

    // update the object list
    obj_list.head = new_node;
    obj_list.count++;

    return 0;
}

//  Purpose: Remove node bound to `object` from `obj_list`.
//  Input Assumptions: None.
//  Effects: Free the node and update `obj_list.count`
//  Returns:
//    0: Success.
//    1: Invalid input.
//    2: Not used in this function.
//    3: Internal invariant violation.
//  Notes:
//    - Enforces invariant: Removal not allowed from empty `obj_list`.
//    - Enforces invariant: `obj_list.head` must exist.
static int remove_obj(struct ObjWrapper* object)
{
    if (!object)
        return 1; // caller error
    if (obj_list.count == 0 || obj_list.head == NULL)
        return 3; // internal error

    struct ObjLLNode* prev_node = NULL;
    struct ObjLLNode* search_node = find_node(object, prev_node);

    // node found
    if (search_node)
    {
        // not first element
        if (prev_node)
            prev_node->next = search_node->next;
        // is first element
        else
            obj_list.head = search_node->next;

        // free node and update list count
        free(search_node);
        obj_list.count--;

        return 0;
    }

    // node not found
    return 1;
}

//  Purpose:  Find/return `obj_list` node bound to `wrapper` if it exsits.
//  Input Assumptions:
//    - Valid input assured by add/remove_obj().
//    - Caller must provide Null `prev_node`
//  Effects: None.
//  Returns:
//    ObjLLNode*: On success.
//    NULL: Not found.
//  Notes:
//    `prev_node` populated if `wrapper` found and `wrapper` is not the first
//    list element.
static struct ObjLLNode* find_node(struct ObjWrapper* wrapper, struct ObjLLNode* prev_node)
{
    struct ObjLLNode* search_node = obj_list.head;
    while (search_node)
    {
        if (search_node->object == wrapper)
            return search_node;
        else
            prev_node = search_node;
        search_node = search_node->next;
    }
    return NULL;
}
#pragma endregion