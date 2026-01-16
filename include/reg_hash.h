#ifndef REG_HASH_H
#define REG_HASH_H

#include <stdlib.h>

/* ============================================================================
 * Module overview / invariants
 * ============================================================================
  - Registry owns bindings (nodes and names); callers never own binding storage.
  - The registry is the sole lifetime manager for objects via reference
    counting.
  - Creating or updating a binding increments an object's refcount; unbinding
    decrements it.
  - When a decrement causes an object’s refcount to reach zero, the object is
    destroyed.
  - Any attempt to decrement an object whose refcount is already zero
    is an invariant violation and constitutes an internal registry error.
  - Unless otherwise specified, functions that return int return 0 on success
    and nonzero on error; specific codes are documented per function.
 */

/* ============================================================================
 * Public types
 * ============================================================================
 */
struct RegistryHash;

/* ============================================================================
 * Public API
 * ============================================================================
 */

/**
@brief
  Create an empty registry hash table.
@param table_size num elements in hash table; must be > 0.
@return
  Returns struct RegistryHash* on success.
  Returns NULL on allocation failure or invalid table_size.
@pre
  table_size > 0
@post
  Table is initialized and safe to pass to other registry API calls.
@note
  Caller owns the returned registry and must destroy it with
  destroy_reg_table().*/
struct RegistryHash* init_reg_table(size_t table_size);

/**
@brief
  Destroy the registry and all bindings.
@param reg_table Registry hash table.
@return
  0: success (including NULL no-op).
@pre None
@post
  If reg_table == NULL: then no side effects.
  Otherwise:
    All registry-owned memory (table, nodes, names) is freed
    All bindings are removed.
    Each removal calls decref_obj() on the bound object.
    ref_count and object destruction are managed in math_objs API.
@note
  Safe to call with NULL.
  After return, the pointer must not be used.
@warning
  Will destroy all objects bound in the registry table.
 */
int destroy_reg_table(struct RegistryHash* reg_table);

/**
@brief
  Add or update a binding (name->object).
@param name Binding name (null-terminated).
@param object Object wrapper to bind to. Registry holds/releases references via
  incref_obj()/decref_obj()
@param reg_table Registry hash table.
@return
  0 if success.
  1 if add_node() failure.
  2 if failed allocation.
  3 if invalid input.
  4 not used.
  5 if incref_obj() failure.
@pre
  reg_table != NULL.
  name != NULL and name[0] != '\0'.
  object != NULL.
@post
  If name was unbound: name points to object and incref_obj(object) called.
  If name was bound to the same object: no-op, no incref_obj()/decref_obj()
    calls.
  If name was bound to a different object: After success name points to
    object. Registry releases its reference to the previously bound object and
    acquires a reference to the provided object via incref_obj()/decref_obj() as
    needed.
@note
  On allocation failure: no new binding exists, no binding is modified,
  incref_obj()/decref_obj() not called.
@warning
  Registry's decref_obj() call may destroy the previously bound object if
  overwritten.*/
int add_binding(const char* name, struct ObjWrapper* object, struct RegistryHash* reg_table);

/**
@brief Remove a binding by name.
@param name Name of binding to remove (null-terminated).
@param reg_table Registry table of name bindings.
@return
  0: Success. Binding existed and was removed.
  1: Binding not found.
  2: Reserved (not returned by this function).
  3: Invalid or empty reg_table.
  4: Internal registry error.
@pre
  name != NULL.
  name[0] != '\0'.
@post
  If binding exists:
    Registry removes the binding, then calls decref_obj() on the previously
    bound object.
    All binding storage associated with name is released.
  If binding does not exist:
    No bindings are modified and no decref_obj() call occurs.
@warning
  Call to decref_obj() may destroy the bound object.
 */
int remove_binding(const char* name, struct RegistryHash* reg_table);

/**
@brief
  Fetch the object bound to name.
@param name Binding name (null-terminated).
@param reg_table Registry table of name bindings.
@return
 struct ObjWrapper*: Binding found.
 NULL: Invalid input or binding not found.
@pre
 reg_table != NULL.
 name != NULL.
 name[0] != '\0'.
@post No side effects; does not modify registry or refcounts.
@note Returned pointer is borrowed; caller must not free/destroy it.
 */
struct ObjWrapper* lookup_binding(const char* name, struct RegistryHash* reg_table);

/**
@brief
  Print all binding info from reg_table
@param reg_table Registry table of name bindings.
@return
  0: Success.
  1: Invalid input.
@pre
  reg_table != NULL.
@post None.
@note Prints out name, ref_count, and address for all bindings
 */
int list_bindings(struct RegistryHash* reg_table);

#endif // REG_HASH_H