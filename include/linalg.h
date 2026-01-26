#ifndef LINALG_H
#define LINALG_H

#include <stddef.h>
#include <stdlib.h>

#include "linalg_types.h"

/**
 * =====================================================================
 * Create+Bind Semantics (applies to linalg_create_bind_matrix/vector/scalar)
 * =====================================================================
 *
 * These functions create a new math object and bind it to `name` as a single
 * operation. The caller does not receive or manage an object pointer; `name`
 * is the sole public handle used to reference the object via other linalg APIs.
 *
 * Initialization:
 *   - linalg_init_reg_table() must have succeeded before calling any
 *     linalg_create_bind_*() function.
 *
 * Name binding behavior:
 *   - On success, `name` is bound to the newly created object.
 *   - If `name` is already bound, the existing binding is replaced and the
 *     previously bound object may be destroyed as a result.
 *
 * Ownership / lifetime:
 *   - For matrix/vector creation, ownership of `elements.list` transfers to the
 *     library on success; the caller must not free `elements.list` after a
 *     successful call.
 *   - On failure, no created object is retained and ownership of caller-provided
 *     inputs remains with the caller (i.e., no net resource leak is introduced
 *     by a failed call).
 *   - All remaining bound objects are released during linalg_shutdown().
 *
 * Return codes (common to all linalg_create_bind_*()):
 *   0: Success.
 *   1: Invalid input or invalid library state (e.g., not initialized).
 *   2: Allocation/creation failure.
 *   3: Internal/unspecified failure.
 */

/**
 @brief Creates new matrix and binds it to name.
 @param elements: matrix element values.
 @param num_rows: number of matrix rows.
 @param num_cols: number of matrix cols.
 @param name: binding name for created matrix.
 @return
    0: Success.
    1: Invalid input.
    2: Allocation failure.
    3: Internal error.
 @pre
    1. name != NULL and name[0] != '\0'.
    2. elements.list != NULL.
    3. elements.type_size > 0.
    4. num_rows > 0, num_cols > 0.
    5. elements.size == num_rows * num_cols.
 */
int linalg_create_bind_matrix(struct List elements, size_t num_rows, size_t num_cols,
                              const char* name);

/**
 @brief Creates new vector and binds it to name.
 @param elements: vector element values.
 @param name: binding name for created vector
 @return
    0: Success.
    1: Invalid input.
    2: Allocation failure.
    3: Internal error.
 @pre
    1. name != NULL and name[0] != '\0'.
    2. elements.list != NULL.
    3. elements.size > 0.
    4. elements.type_size > 0.
 */
int linalg_create_bind_vector(struct List elements, const char* name);

/**
 @brief Creates new scalar and binds it to name.
 @param value: numerical value of scalar.
 @param name: binding name for created scalar.
 @return
    0: Success.
    1: Invalid input.
    2: Allocation failure.
    3: Internal error.
 @pre
    - name != NULL and name[0] != '\0'.
 */
int linalg_create_bind_scalar(double value, const char* name);

/**
@brief:
  Perform final teardown and release all held objects.
@return:
  0: In all cases.
@note: Used exclusively for program/session shutdown.
 */
int linalg_shutdown(void);

/**
 @brief
    Initializes the library's internal name registry.
 @param table_size num elements in hash table; must be > 0.
 @return
    0: Success.
    2: Failure.
 @pre
  table_size > 0
 @post
    Registry is initialized and safe to create+bind APIs.
 @note
    - Registry is internal and released by linalg_shutdown().
*/
int linalg_init_reg_table(size_t table_size);

/**
 @brief Release a binding by name.
 @param name: Name of binding to remove (null-terminated).
 @return
   0: Success. Binding existed and was removed.
   1: Invalid input.
   2: Reserved (not returned by this function).
   3: Internal error.
 @pre
   - name != NULL.
   - name[0] != '\0'.
 @post
    - If binding exists:
        - Binding for `name` is removed.
        - All binding storage associated with name is released.
    - If binding does not exist:
        - No bindings are modified.
 @warning
  Object will be destroyed if this is only reference to object.
 */
int linalg_remove_binding(const char* name);

#endif // LINALG_H