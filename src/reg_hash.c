#include "reg_hash.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "logs.h"
#include "math_objs.h"

#pragma region Head Comment
/*
 * Translation unit implements:
 * - Creation, operations, and teardown of the name-binding hash table.
 * - Coordination with the math_objs API via incref_obj() and decref_obj()
 *   for reference-counted object lifetime management.
 * - Lookup functionality for retrieving bindings by name.
 *
 * Registry invariants:
 * - Registry owns all stored name strings.
 * - Successful binding retains the associated ObjWrapper
 *   via incref_obj().
 * - Unbinding releases the associated ObjWrapper
 *   via decref_obj().
 * - Overwrite behavior:
 *     - Existing binding is released (decref_obj()).
 *     - New binding is retained (incref_obj()).
 * - Overwrite with the same ObjWrapper is a no-op.
 *
 * Internal conventions:
 * - None.
 */
#pragma endregion

#pragma region Local Definitions
/* ============================================================================
 * File-local definitions
 * ============================================================================
 */
struct RegistryLL
{
    struct ObjWrapper* object; // non-owning, lifetime via ref_count
    char* name;                // owning, must free on unbind
    struct RegistryLL* next;
};

struct RegistryHash
{
    struct RegistryLL** table;
    size_t size;
};
#pragma endregion

#pragma region Private Function Prototypes
/* ============================================================================
 * Private function prototypes
 * ============================================================================
 */

static unsigned int hash(const char* s); // from K&R 'C programming language'
static struct RegistryLL* find_node(struct RegistryLL** prev_node, struct RegistryHash* reg_table,
                                    const char* name, size_t index);
static char* copy_name(const char* name);
static int add_node(struct RegistryLL* new_node, struct RegistryLL** list_head);
static int remove_node(struct RegistryLL* node, struct RegistryLL* prev_node,
                       struct RegistryLL** list_head);
static int add_binding_already_bound(struct ObjWrapper* new_wrapper, struct ObjWrapper** slot);
static int add_binding_new_binding(const char* name, struct ObjWrapper* new_wrapper,
                                   struct RegistryHash* reg_table, size_t index);
static int free_registry_node(struct RegistryLL* node);
#pragma endregion

#pragma region Public API

/* ============================================================================
 * Public API implementation
 * ============================================================================
 */

// Allocates Binding Registry structure and hash table.
// Caller must use destroy_reg_table() for cleanup.
// Allocation failures return NULL
struct RegistryHash* init_reg_table(size_t table_size)
{
    if (table_size == 0)
        return NULL; // caller error

    // allocate for table
    struct RegistryHash* reg_table = malloc(sizeof(struct RegistryHash));
    if (!reg_table)
    {
        LOG_OUT(LOG_ERROR, "failed to allocate %zu bytes for reg table of size %zu.",
                sizeof(struct RegistryHash), table_size);
        return NULL;
    }

    // allocate for table buckets
    // calloc -> initialize to zero
    struct RegistryLL** table = calloc(table_size, sizeof(struct RegistryLL*));
    if (!table)
    {
        LOG_OUT(LOG_ERROR,
                "failed to allocate %zu bytes for table buckets for reg_table of size %zu.",
                table_size * sizeof(struct RegistryLL*), table_size);
        free(reg_table);
        return NULL;
    }

    // populate reg_table struct on success
    reg_table->table = table;
    reg_table->size = table_size;

    LOG_OUT(LOG_DEBUG, "success: reg_table=%p size=%zu.", reg_table, reg_table->size);
    return reg_table;
}

// Returns 0 regardless of success/failure.
int destroy_reg_table(struct RegistryHash* reg_table)
{
    if (!reg_table)
        return 0; // return 0 if passed NULL

    LOG_OUT(LOG_DEBUG, "reg_table teardown beginning table=%p size=%zu.", reg_table,
            reg_table->size);

    size_t decref_obj_count = 0;
    size_t free_node_count = 0;

    for (size_t i = 0; i < reg_table->size; i++)
    {
        struct RegistryLL* node = reg_table->table[i];
        struct RegistryLL* next_node = NULL;

        while (node)
        {
            int decref_ret = decref_obj(node->object);
            if (decref_ret != 0)
            {
                LOG_OUT(LOG_ERROR, "decref_obj() failed ptr=%p name=%s slot=%zu rtn=%d.",
                        node->object, node->name, i, decref_ret);
                assert(decref_ret == 0);
            }
            else
                decref_obj_count++;
            next_node = node->next;
            free_registry_node(node);
            free_node_count++;
            node = next_node;
        }
    }

    LOG_OUT(LOG_DEBUG,
            "reg_table teardown ended table=%p size=%zu decref_count=%zu free_node_count=%zu.",
            reg_table, reg_table->size, decref_obj_count, free_node_count);

    free(reg_table->table);
    free(reg_table);
    return 0;
}

int add_binding(const char* name, struct ObjWrapper* object, struct RegistryHash* reg_table)
{
    // return immediately on invalid input
    if (!name || name[0] == '\0' || !object || !reg_table || reg_table->size == 0)
        return 1; // caller error

    // local defines
    size_t index = hash(name) % reg_table->size;
    struct RegistryLL* prev_node = NULL;
    struct RegistryLL* already_bound = find_node(&prev_node, reg_table, name, index);

    // if name already bound
    if (already_bound)
    {
        return add_binding_already_bound(object, &already_bound->object);
    }

    // if name is not bound create new node
    return add_binding_new_binding(name, object, reg_table, index);
}

int remove_binding(const char* name, struct RegistryHash* reg_table)
{
    // return immediately on invalid input
    if (!name || name[0] == '\0' || !reg_table || reg_table->size == 0)
        return 3; // caller error

    // local defines
    size_t index = hash(name) % reg_table->size;
    struct RegistryLL* prev_node = NULL;
    struct RegistryLL* found_node = find_node(&prev_node, reg_table, name, index);

    // binding not found or missing wrapper
    if (!found_node)
        return 1;
    if (!found_node->object)
    {
        LOG_OUT(LOG_ERROR, "missing object name=%s node_ptr=%p slot=%zu.", name, found_node, index);
        return 4; // internal registry error
    }

    // remove/free the node
    remove_node(found_node, prev_node, &reg_table->table[index]);
    struct ObjWrapper* node_object =
        found_node->object; // store for freeing after found_node released
    free_registry_node(found_node);

    // decrement the node wrapper
    LOG_OUT(LOG_DEBUG, "calling decref_obj() obj=%p name=%s", node_object, name);
    int decref_ret = decref_obj(node_object);
    if (decref_ret != 0)
    {
        LOG_OUT(LOG_ERROR, "decref_obj() failed rtn=%d name=%s obj=%p slot=%zu.", decref_ret, name,
                node_object, index);
        assert(decref_ret == 0); // internal invariant violation
    }

    return 0;
}

struct ObjWrapper* lookup_binding(const char* name, struct RegistryHash* reg_table)
{
    if (!name || name[0] == '\0' || !reg_table || reg_table->size == 0)
        return NULL; // caller error
    size_t table_size = reg_table->size;
    size_t index = hash(name) % table_size;
    struct RegistryLL* prev = NULL;

    struct RegistryLL* node = find_node(&prev, reg_table, name, index);

    if (!node)
        return NULL;

    return node->object;
}

// Diagnostic: prints current registry bindings; no side effects
int list_bindings(struct RegistryHash* reg_table)
{
    if (!reg_table)
        return 1; // no reg_table
    if (!reg_table->table)
        return 1; // no table

    size_t table_size = reg_table->size;
    struct RegistryLL** table = reg_table->table;
    struct RegistryLL* node;

    for (size_t i = 0; i < table_size; i++)
    {
        node = table[i];
        while (node)
        {
            printf("%s -> %p (type=%d, refs=%zu)\n", node->name, (void*)node->object,
                   get_obj_type(node->object), debug_get_obj_refcount(node->object));
            node = node->next;
        }
    }
    return 0;
}
#pragma endregion

#pragma region Private Functions
/* ============================================================================
 * Private helper implementation
 * ============================================================================
 */

//  Purpose: Hash func for binding registry.
//  Input assumptions:
//    s: null-terminated.
//  Effects: None
//  Returns:
//    h: unsigned hash of s.
//  Note: from K&R 'The C Programming Language'
static unsigned int hash(const char* s)
{
    unsigned int h = 0;
    while (*s)
    {
        h = h * 31 + *s++;
    }
    return h;
}

//  Purpose: Search for registry node by name.
//  Input assumptions:
//    prev_node: Previous node container provided by caller.
//    index: Hash result on name provided by caller.
//  Effects:
//    prev_node populated if name not first element.
//  Returns:
//    Success: Registry node.
//    Not found: NULL.
//  Note: prev_node only valid on non_NULL return.
static struct RegistryLL* find_node(struct RegistryLL** prev_node, struct RegistryHash* reg_table,
                                    const char* name, size_t index)
{
    struct RegistryLL** table = reg_table->table;
    *prev_node = NULL;

    struct RegistryLL* node = table[index];
    while (node)
    {
        if (!strcmp(name, node->name))
        {
            return node;
        }
        *prev_node = node;
        node = node->next;
    }

    return NULL;
}

//  Purpose: Allocate and return copy of name.
//  Input assumptions:
//    name: NULL terminated.
//  Effects: Heap allocation of name.
//  Returns:
//    Success: copied name char*
//    Allocation failure: NULL
//  Note: Copied name is owned/freed by registry node
static char* copy_name(const char* name)
{
    size_t str_len = strlen(name) + 1;
    char* name_copy = malloc(str_len);
    if (!name_copy)
        return NULL;
    strcpy(name_copy, name);
    return name_copy;
}

//  Purpose: Links `new_node` to the list headed by `*list_head`.
//  Input assumptions: `new_node` and `list_head` exist.
//  Effects: Registry extended with new node.
//  Returns:
//    0: Success.
//    1: Invalid input.
//  Note:
static int add_node(struct RegistryLL* new_node, struct RegistryLL** list_head)
{
    if (!new_node || !list_head)
        return 1; // call error
    new_node->next = *list_head;
    *list_head = new_node;

    return 0;
}

// Unlinks `node` from the list headed by `*list_head`.
// Does NOT free `node` or `node->name`.
// Caller is responsible for freeing memory and handling refcounts.
static int remove_node(struct RegistryLL* node, struct RegistryLL* prev_node,
                       struct RegistryLL** list_head)
{
    if (!node || !list_head)
        return 3; // call error
    if (!prev_node)
        *list_head = node->next;
    else
        prev_node->next = node->next;

    return 0;
}

//  Purpose: Helper function for add_binding() for name already bound.
//  Input assumptions: Valid input assured by add_binding()
//  Effects:
//    - Nothing if `old_wrapper` == `new_wrapper`
//    - Otherwise increment `new_wrapper`, decrement `old_wrapper`.
//      Replace `old_wrapper` with `new_wrapper` in registry.
//  Returns:
//    0: On success.
//    5: On failed incref_obj()
//  Notes: Failed decref_obj() is an invariant violation
static int add_binding_already_bound(struct ObjWrapper* new_wrapper, struct ObjWrapper** slot)
{
    if (new_wrapper == *slot)
    { // already bound to object
        LOG_OUT(LOG_DEBUG, "duplicate wrapper binding ignored wrapper=%p.", new_wrapper);
        return 0;
    }
    else
    { // name is bound to a different object (overwrite)

        struct ObjWrapper* old_wrapper = *slot;

        // increment new wrapper
        int incref_ret = incref_obj(new_wrapper);
        if (incref_ret != 0)
        {
            LOG_OUT(LOG_ERROR, "incref_obj failure wrapper=%p ret=%d.", new_wrapper, incref_ret);
            return 5; // incref failure
        }
        // replace found node object with caller `object`
        *slot = new_wrapper;

        // decrement previously bound wrapper
        LOG_OUT(LOG_DEBUG, "calling decref_obj() after overwrite of ptr=%p type=%d", old_wrapper,
                get_obj_type(old_wrapper));
        int decref_ret = decref_obj(old_wrapper);
        if (decref_ret != 0)
        {
            LOG_OUT(LOG_ERROR, "decref_obj failure wrapper=%p ret=%d.", old_wrapper, decref_ret);
            assert(decref_ret == 0); // invariant violation
            return 3;
        }

        LOG_OUT(LOG_DEBUG, "registry overwrite occurred: old=%p new=%p type=%d.", old_wrapper,
                *slot, get_obj_type(new_wrapper));
        return 0;
    }
}

//  Purpose: Helper function for add_binding() when name is not already bound.
//  Input Assumptions: Valid input already assured by add_binding().
//  Effects: New node added to Registry linked list.
//  Returns:
//    0: Success.
//    1: add_node() failure.
//    2: Allocation failure.
//    5: Incref_obj() failure.
//  Notes: None.
static int add_binding_new_binding(const char* name, struct ObjWrapper* new_wrapper,
                                   struct RegistryHash* reg_table, size_t index)
{
    char* new_name = copy_name(name);
    if (!new_name)
    {
        LOG_OUT(LOG_ERROR, "failed to copy name=%s for ptr=%p", name, new_wrapper);
        return 2; // allocation failure
    }

    struct RegistryLL* new_node = malloc(sizeof(struct RegistryLL));
    if (!new_node)
    {
        LOG_OUT(LOG_ERROR, "failed to allocate %zu bytes for registry node.",
                sizeof(struct RegistryLL));
        free(new_name);
        return 2; // allocation failure
    }

    // populate new node
    new_node->name = new_name;
    new_node->next = NULL;
    new_node->object = new_wrapper;

    // increment new wrapper
    int incref_ret = incref_obj(new_node->object);
    if (incref_ret)
    {
        LOG_OUT(LOG_ERROR, "incref_obj() failed with ret=%d.", incref_ret);
        free_registry_node(new_node);
        return 5; // incref failure
    }

    // add new node
    int add_node_return = add_node(new_node, &reg_table->table[index]);
    // successfully added node
    if (add_node_return == 0)
    {
        LOG_OUT(LOG_DEBUG, "added object binding name=%s ptr=%p slot=%zu.", name, new_wrapper,
                index);
        return 0;
    }

    // failed to add new node
    else
    {
        LOG_OUT(LOG_ERROR, "add_node() failed name=%s ptr=%p ret=%d slot=%zu calling decref_obj().",
                name, new_node, add_node_return, index);
        int decref_ret = decref_obj(new_wrapper);
        free_registry_node(new_node);
        assert(decref_ret == 0); // invariant violation
        return 3;
    }
}

//  Purpose: Helper function to release 'node' and 'node->name'.
//  Input Assumptions: Caller assures `node` and `node->name` exist.
//  Effects: `node` and `node->name` are freed.
//  Returns: 0 in all cases.
//  Notes: None.
static int free_registry_node(struct RegistryLL* node)
{
    free(node->name);
    free(node);

    return 0;
}
#pragma endregion