#ifndef MATH_OBJS_H
#define MATH_OBJS_H

#include <stdlib.h>

#include "./linalg.h"

/* ============================================================================
 * Public types
 * ============================================================================
 */
enum ObjType
{
    OBJ_SCALAR,
    OBJ_VECTOR,
    OBJ_MATRIX,
};

struct ObjWrapper;
struct Matrix;
struct Vector;
struct Scalar;
struct ObjLL;

/* ============================================================================
 * Public API
 * ============================================================================
 */

/**
@brief
  Create a matrix object from a caller-provided element buffer.
@param elements: Numeric list of matrix elements.
@param num_rows: Number of rows.
@param num_cols: Number of columns.
@return
  ObjWrapper*:  to newly created ObjWrapper on success.
  NULL: on any allocation or validation failure.
@pre
  elements.list != NULL.
  elements.type_size > 0.
  num_rows > 0.
  num_cols > 0.
  elements.size == num_rows * num_cols.
@post None.
@note
  - Object destruction occurs when the final reference is released via
    `decref_obj()`.
 */
struct ObjWrapper *create_matrix(struct List elements, size_t num_rows,
                                 size_t num_cols);

/**
@brief
  Create a vector object from a caller-provided element buffer.
@param elements: Numeric list of vector elements.
@return
  ObjWrapper*:  to newly created ObjWrapper on success.
  NULL: on any allocation or validation failure.
@pre
  elements.list != NULL.
  elements.type_size > 0.
  elements.size > 0.
@post None.
@note
  - All created objects are registered with the root set immediately.

  - The caller is responsible for releasing additional references via the
    binding registry; object destruction occurs only when ref_count reaches
    0.
@warning None.
 */
struct ObjWrapper *create_vector(struct List elements);

/**
@brief
  Create new scalar object with value given by `value`.
@param value: Numerical value of scalar.
@return struct
  ObjWrapper*: On success.
  NULL: On allocation or validation error.
@pre None.
@post None.
@note
  - All created objects are registered with the root set immediately.

  - The caller is responsible for releasing additional references via the
    binding registry; object destruction occurs only when ref_count reaches
    0.
@warning
 */
struct ObjWrapper *create_scalar(double value);

/**
@brief
  Return `type` field for passed wrapper.
@param wrapper: Object wrapper for type inquiry.
@return enum
  ObjType: In all cases.
@pre None.
@post None.
@note Used in debug function `list_bindings()`
@warning
 */
enum ObjType get_obj_type(const struct ObjWrapper *wrapper);

/**
@brief
  Perform decrementing and possibly deletion of wrappers.
@param wrapper: Object wrapper to be decremented.
@return
  0: On success.
  1: Invalid input.
  2: Not used in this function.
  3: Internal invariant violation.
@pre
  wrapper != NULL.
@post None.
@note Enforces invariant: Cannot decrement `wrapper` with `ref_count` == 0.
@warning
 */
int decref_obj(struct ObjWrapper *wrapper);

/**
@brief
  Perform incrementing of object wrappers.
@param wrapper: Object wrapper to increment.
@return
  0: Success.
  1: Invalid input.
  2: Not used in this function.
  3: Internal invariant error.
@pre
  wrapper != NULL.
  wrapper->ref_count != 0.
@post None.
@note Enforces invariant: Active object `ref_count` > 0.
@warning None.
 */
int incref_obj(struct ObjWrapper *wrapper);

/**
@brief
  Perform final teardown of `obj_list`.
@return
  0: In all cases.
@pre
  `object->ref_count` == 1 for all existing objects.
@post
  `obj_list.count` == 0.
  `obj_list.head` == NULL.
@note Used exclusively for program/session shutdown.
@warning None.
 */
int destroy_obj_list();

/* ============================================================================
 * Public debug functions
 * ============================================================================
 */

/**
@brief
  Return the `ref_count` field assocated with `wrapper`.
@param wrapper: Object to query for `ref_count`.
@return
  size_t: In all cases.
@pre
  wrapper != NULL.
@post None.
@note None.
@warning None.
 */
size_t debug_get_obj_refcount(const struct ObjWrapper *wrapper);

#endif // MATH_OBJS_H