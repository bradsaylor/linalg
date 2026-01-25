#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

#include "math_objs.h"
#include "reg_hash.h"

/* ============================================================================
 * File-local definitions
 * ============================================================================
 */
#define TABLE_SIZE 256
#define DELIM "********************************************\n"

/* ============================================================================
 * Test function prototypes
 * ============================================================================
 */
int test_init_reg_table();
int test_add_binding_different_object();
int test_add_binding_same_object();
int test_add_binding_same_name_noop();
int test_remove_then_rebind_new_name();
int test_add_binding_overwrite_different_object();
int test_remove_existing_binding();
int test_remove_nonexistent_binding();
int test_remove_binding_invalid_name();
int test_remove_binding_invalid_table();
int test_destroy_reg_table_null_noop();
int test_destroy_reg_table_nonempty_returns_0();
int test_lookup_existing_binding();
int test_lookup_binding_invalid_name();
int test_lookup_binding_invalid_table();
int test_list_bindings();

/* ============================================================================
 * main()
 * ============================================================================
 */
int main()
{
    assert(test_init_reg_table() == 0);
    assert(test_add_binding_different_object() == 0);
    assert(test_add_binding_same_object() == 0);
    assert(test_add_binding_same_name_noop() == 0);
    assert(test_remove_then_rebind_new_name() == 0);
    assert(test_add_binding_overwrite_different_object() == 0);
    assert(test_remove_existing_binding() == 0);
    assert(test_remove_nonexistent_binding() == 0);
    assert(test_remove_binding_invalid_name() == 0);
    assert(test_remove_binding_invalid_table() == 0);
    assert(test_destroy_reg_table_null_noop() == 0);
    assert(test_destroy_reg_table_nonempty_returns_0() == 0);
    assert(test_lookup_existing_binding() == 0);
    assert(test_lookup_binding_invalid_name() == 0);
    assert(test_lookup_binding_invalid_table() == 0);
    assert(test_list_bindings() == 0);

    return 0;
}

/* ============================================================================
 * Test function implementations.
 * ============================================================================
 */

int test_init_reg_table()
{
    const char* test_name = "test_init_reg_table()";
    const char* lookup_name = "test";
    struct RegistryHash* reg_table = init_reg_table(TABLE_SIZE);
    // bools equals1 for passing 0 for failing
    bool init_ok = false;
    bool lookup_ok = true;
    bool remove_ok = true;
    bool destroy_ok = false;

    init_ok = (reg_table != NULL);
    if (!init_ok)
        printf("%s FAILED on init_ok.\n%s\n", test_name, DELIM);

    if (init_ok)
    {
        lookup_ok = (lookup_binding(lookup_name, reg_table) == NULL);
        if (!lookup_ok)
            printf("%s FAILED on lookup_ok.\n%s\n", test_name, DELIM);

        remove_ok = (remove_binding(lookup_name, reg_table) == 1); // 1->not found
        if (!remove_ok)
            printf("%s FAILED on remove_ok.\n%s\n", test_name, DELIM);
    }

    destroy_ok = (destroy_reg_table(reg_table) == 0); // this is cleanup step
    if (!destroy_ok)
        printf("%s FAILED on destroy_ok.\n%s\n", test_name, DELIM);

    if (init_ok && lookup_ok && remove_ok && destroy_ok)
    {
        printf("%s PASSED.\n%s\n", test_name, DELIM);
        return 0;
    }
    else
        return 1;

    return 0;
}

int test_add_binding_different_object()
{
    const char* test_name = "test_add_binding";
    struct ObjWrapper* wrapper_ptr = create_scalar(3.14);
    const char* name1 = "A";
    struct RegistryHash* reg_table = init_reg_table(TABLE_SIZE);
    bool unbound_before_add = false;
    bool bound_after_add = false;
    bool add_ok = false;
    bool init_ok = false;

    init_ok = (reg_table != NULL);
    if (!init_ok)
        printf("%s FAILED on init_ok.\n%s\n", test_name, DELIM);

    else
    {
        unbound_before_add = (lookup_binding(name1, reg_table) == NULL);
        if (!unbound_before_add)
            printf("%s FAILED on unbound_before_add.\n%s\n", test_name, DELIM);

        add_ok = (add_binding(name1, wrapper_ptr, reg_table) == 0);
        if (!add_ok)
            printf("%s FAILED on add_ok.\n%s\n", test_name, DELIM);

        bound_after_add = (lookup_binding(name1, reg_table) == wrapper_ptr);
        if (!bound_after_add)
            printf("%s FAILED on bound_after_add.\n%s\n", test_name, DELIM);
    }

    destroy_reg_table(reg_table);

    if (init_ok && unbound_before_add && add_ok && bound_after_add)
    {
        printf("%s PASSED.\n%s\n", test_name, DELIM);
        return 0;
    }
    else
        return 1;
}

int test_add_binding_same_object()
{
    const char* test_name = "test_add_binding_same_object";
    struct ObjWrapper* wrapper_ptr = create_scalar(3.14);
    const char* name1 = "A";
    const char* name2 = "B";
    struct RegistryHash* reg_table = init_reg_table(TABLE_SIZE);
    bool init_ok = false;
    bool unbound_before_add = false;
    bool bound_after_add1 = false;
    bool add1_ok = false;
    bool add2_ok = false;
    bool binding_unchanged_add2 = false;

    init_ok = (reg_table != NULL);
    if (!init_ok)
        printf("%s FAILED on init_ok.\n%s\n", test_name, DELIM);

    else
    {
        unbound_before_add = (lookup_binding(name1, reg_table) == NULL);
        if (!unbound_before_add)
            printf("%s FAILED on unbound_before_add.\n%s\n", test_name, DELIM);

        add1_ok = (add_binding(name1, wrapper_ptr, reg_table) == 0);
        if (!add1_ok)
            printf("%s FAILED on add1_ok.\n%s\n", test_name, DELIM);

        bound_after_add1 = (lookup_binding(name1, reg_table) == wrapper_ptr);
        if (!bound_after_add1)
            printf("%s FAILED on bound_after_add1.\n%s\n", test_name, DELIM);

        add2_ok = (add_binding(name2, wrapper_ptr, reg_table) == 0);
        if (!add2_ok)
            printf("%s FAILED on add2_ok.\n%s\n", test_name, DELIM);

        binding_unchanged_add2 = (lookup_binding(name2, reg_table) == wrapper_ptr);
        if (!binding_unchanged_add2)
            printf("%s FAILED on binding_unchanged_add2.\n%s\n", test_name, DELIM);
    }

    destroy_reg_table(reg_table);

    if (init_ok && unbound_before_add && add1_ok && bound_after_add1 && add2_ok &&
        binding_unchanged_add2)
    {
        printf("%s PASSED.\n%s\n", test_name, DELIM);
        return 0;
    }
    else
    {
        return 1;
    }
}

int test_add_binding_same_name_noop()
{
    const char* test_name = "test_add_binding_same_name_noop";
    struct ObjWrapper* wrapper_ptr = create_scalar(3.14);
    const char* name1 = "A";
    struct RegistryHash* reg_table = init_reg_table(TABLE_SIZE);
    bool init_ok = false;
    bool unbound_before_add = false;
    bool bound_after_add1 = false;
    bool add1_ok = false;
    bool add2_ok = false;
    bool bound_name2_after_add2 = false;

    init_ok = (reg_table != NULL);
    if (!init_ok)
        printf("%s FAILED on init_ok.\n%s\n", test_name, DELIM);

    else
    {
        unbound_before_add = (lookup_binding(name1, reg_table) == NULL);
        if (!unbound_before_add)
            printf("%s FAILED on unbound_before_add.\n%s\n", test_name, DELIM);

        add1_ok = (add_binding(name1, wrapper_ptr, reg_table) == 0);
        if (!add1_ok)
            printf("%s FAILED on add1_ok.\n%s\n", test_name, DELIM);

        bound_after_add1 = (lookup_binding(name1, reg_table) == wrapper_ptr);
        if (!bound_after_add1)
            printf("%s FAILED on bound_after_add1.\n%s\n", test_name, DELIM);

        add2_ok = (add_binding(name1, wrapper_ptr, reg_table) == 0);
        if (!add2_ok)
            printf("%s FAILED on add2_ok.\n%s\n", test_name, DELIM);

        bound_name2_after_add2 = (lookup_binding(name1, reg_table) == wrapper_ptr);
        if (!bound_name2_after_add2)
            printf("%s FAILED on bound_name2_after_add2.\n%s\n", test_name, DELIM);
    }

    destroy_reg_table(reg_table);

    if (init_ok && unbound_before_add && add1_ok && bound_after_add1 && add2_ok &&
        bound_name2_after_add2)
    {
        printf("%s PASSED.\n%s\n", test_name, DELIM);
        return 0;
    }
    else
    {
        return 1;
    }
}

int test_remove_then_rebind_new_name()
{
    const char* test_name = "test_remove_then_rebind_new_name";
    struct ObjWrapper* wrapper_ptr = create_scalar(3.14);
    const char* name1 = "A";
    const char* name2 = "B";
    struct RegistryHash* reg_table = init_reg_table(TABLE_SIZE);
    bool init_ok = false;
    bool unbound_before_add1 = false;
    bool add1_ok = false;
    bool bound_name1_after_add1 = false;
    bool unbound_after_remove = false;
    bool add2_ok = false;
    bool bound_name2_after_add2 = false;

    init_ok = (reg_table != NULL);
    if (!init_ok)
        printf("%s FAILED on init_ok.\n%s\n", test_name, DELIM);
    else
    {
        unbound_before_add1 = (lookup_binding(name1, reg_table) == NULL);
        if (!unbound_before_add1)
            printf("%s FAILED on unbound_before_add1.\n%s\n", test_name, DELIM);

        add1_ok = (add_binding(name1, wrapper_ptr, reg_table) == 0);
        if (!add1_ok)
            printf("%s FAILED on add1_ok.\n%s\n", test_name, DELIM);

        bound_name1_after_add1 = (lookup_binding(name1, reg_table) == wrapper_ptr);
        if (!bound_name1_after_add1)
            printf("%s FAILED on bound_name1_after_add1.\n%s\n", test_name, DELIM);

        unbound_after_remove = (remove_binding(name1, reg_table) == 0);
        if (!unbound_after_remove)
            printf("%s FAILED on unbound_after_remove.\n%s\n", test_name, DELIM);

        add2_ok = (add_binding(name2, wrapper_ptr, reg_table) == 0);
        if (!add2_ok)
            printf("%s FAILED on add2_ok.\n%s\n", test_name, DELIM);

        bound_name2_after_add2 = (lookup_binding(name2, reg_table) == wrapper_ptr);
        if (!bound_name2_after_add2)
            printf("%s FAILED on bound_name2_after_add2.\n%s\n", test_name, DELIM);
    }

    destroy_reg_table(reg_table);

    if (init_ok && unbound_before_add1 && add1_ok && bound_name1_after_add1 &&
        unbound_after_remove && add2_ok && bound_name2_after_add2)
    {
        printf("%s PASSED.\n%s\n", test_name, DELIM);
        return 0;
    }
    else
        return 1;
}

int test_add_binding_overwrite_different_object()
{
    const char* test_name = "test_add_binding_overwrite_different_object";
    const char* name1 = "A";
    struct ObjWrapper* wrapper_ptr1 = create_scalar(3.14);
    struct ObjWrapper* wrapper_ptr2 = create_scalar(9.81);
    struct RegistryHash* reg_table = init_reg_table(TABLE_SIZE);

    bool init_ok = false;
    bool unbound_before_add1 = false;
    bool add1_ok = false;
    bool bound_obj1_after_add1 = false;
    bool add2_ok = false;
    bool bound_obj2_after_add2 = false;

    init_ok = (reg_table != NULL);

    if (!init_ok)
        printf("%s FAILED on init_ok.\n%s\n", test_name, DELIM);
    else
    {
        unbound_before_add1 = (lookup_binding(name1, reg_table) == NULL);
        if (!unbound_before_add1)
            printf("%s FAILED on unbound_before_add1.\n%s\n", test_name, DELIM);

        add1_ok = (add_binding(name1, wrapper_ptr1, reg_table) == 0);
        if (!add1_ok)
            printf("%s FAILED on add1_ok.\n%s\n", test_name, DELIM);

        bound_obj1_after_add1 = (lookup_binding(name1, reg_table) == wrapper_ptr1);
        if (!bound_obj1_after_add1)
            printf("%s FAILED on bound_obj1_after_add1.\n%s\n", test_name, DELIM);

        add2_ok = (add_binding(name1, wrapper_ptr2, reg_table) == 0);
        if (!add2_ok)
            printf("%s FAILED on add2_ok.\n%s\n", test_name, DELIM);

        bound_obj2_after_add2 = (lookup_binding(name1, reg_table) == wrapper_ptr2);
        if (!bound_obj2_after_add2)
            printf("%s FAILED on bound_obj2_after_add2.\n%s\n", test_name, DELIM);
    }

    destroy_reg_table(reg_table);

    if (init_ok && unbound_before_add1 && add1_ok && bound_obj1_after_add1 && add2_ok &&
        bound_obj2_after_add2)
    {
        printf("%s PASSED.\n%s\n", test_name, DELIM);
        return 0;
    }
    else
        return 1;
}

int test_remove_existing_binding()
{
    const char* test_name = "test_remove_existing_binding";
    const char* name1 = "A";
    struct ObjWrapper* wrapper_ptr = create_scalar(3.14);
    struct RegistryHash* reg_table = init_reg_table(TABLE_SIZE);

    bool init_ok = false;
    bool unbound_before_add = false;
    bool add_ok = false;
    bool bound_after_add = false;
    bool remove_found_ok = false;
    bool unbound_after_remove = false;

    init_ok = (reg_table != NULL);
    if (!init_ok)
        printf("%s FAILED on init_ok.\n%s\n", test_name, DELIM);
    else
    {
        unbound_before_add = (lookup_binding(name1, reg_table) == NULL);
        if (!unbound_before_add)
            printf("%s FAILED on unbound_before_add.\n%s\n", test_name, DELIM);

        add_ok = (add_binding(name1, wrapper_ptr, reg_table) == 0);
        if (!add_ok)
            printf("%s FAILED on add_ok.\n%s\n", test_name, DELIM);

        bound_after_add = (lookup_binding(name1, reg_table) == wrapper_ptr);
        if (!bound_after_add)
            printf("%s FAILED on bound_after_add.\n%s\n", test_name, DELIM);

        remove_found_ok = (remove_binding(name1, reg_table) == 0);
        if (!remove_found_ok)
            printf("%s FAILED on remove_found_ok.\n%s\n", test_name, DELIM);

        unbound_after_remove = (lookup_binding(name1, reg_table) == NULL);
        if (!unbound_after_remove)
            printf("%s FAILED on unbound_after_remove.\n%s\n", test_name, DELIM);
    }

    destroy_reg_table(reg_table);

    if (init_ok && unbound_before_add && add_ok && bound_after_add && remove_found_ok &&
        unbound_after_remove)
    {
        printf("%s PASSED.\n%s\n", test_name, DELIM);
        return 0;
    }
    else
        return 1;
}

int test_remove_nonexistent_binding()
{
    const char* test_name = "test_remove_nonexistent_binding";
    const char* name1 = "A";
    const char* name2 = "B";
    struct ObjWrapper* wrapper_ptr = create_scalar(3.14);
    struct RegistryHash* reg_table = init_reg_table(TABLE_SIZE);

    bool init_ok = false;
    bool unbound_before_add = false;
    bool add_ok = false;
    bool name1_bound_after_add = false;
    bool remove_nonexistent_name2_ok = false;
    bool name1_still_bound_after_remove_name2 = false;

    init_ok = (reg_table != NULL);
    if (!init_ok)
        printf("%s FAILED on init_ok.\n%s\n", test_name, DELIM);
    else
    {
        unbound_before_add = (lookup_binding(name1, reg_table) == NULL);
        if (!unbound_before_add)
            printf("%s FAILED on unbound_before_add.\n%s\n", test_name, DELIM);

        add_ok = (add_binding(name1, wrapper_ptr, reg_table) == 0);
        if (!add_ok)
            printf("%s FAILED on add_ok.\n%s\n", test_name, DELIM);

        name1_bound_after_add = (lookup_binding(name1, reg_table) == wrapper_ptr);
        if (!name1_bound_after_add)
            printf("%s FAILED on name1_bound_after_add.\n%s\n", test_name, DELIM);

        remove_nonexistent_name2_ok = (remove_binding(name2, reg_table) == 1);
        if (!remove_nonexistent_name2_ok)
            printf("%s FAILED on remove_nonexistent_name2_ok.\n%s\n", test_name, DELIM);

        name1_still_bound_after_remove_name2 = (lookup_binding(name1, reg_table) == wrapper_ptr);
        if (!name1_still_bound_after_remove_name2)
            printf("%s FAILED on name1_still_bound_after_remove_name2.\n%s\n", test_name, DELIM);
    }

    destroy_reg_table(reg_table);

    if (init_ok && unbound_before_add && add_ok && name1_bound_after_add &&
        remove_nonexistent_name2_ok && name1_still_bound_after_remove_name2)
    {
        printf("%s PASSED.\n%s\n", test_name, DELIM);
        return 0;
    }
    else
        return 1;
}

int test_remove_binding_invalid_name()
{
    const char* test_name = "test_remove_binding_invalid_name";
    const char* name1 = "A";
    const char* name_null = NULL;
    const char* name_empty = "";
    struct ObjWrapper* wrapper_ptr = create_scalar(3.14);
    struct RegistryHash* reg_table = init_reg_table(TABLE_SIZE);

    bool init_ok = false;
    bool add_ok = false;
    bool remove_null_returns_3 = false;
    bool remove_empty_returns_3 = false;
    bool good_name_still_bound = false;

    init_ok = (reg_table != NULL);
    if (!init_ok)
        printf("%s FAILED on init_ok.\n%s\n", test_name, DELIM);
    else
    {
        add_ok = (add_binding(name1, wrapper_ptr, reg_table) == 0);
        if (!add_ok)
            printf("%s FAILED on add_ok.\n%s\n", test_name, DELIM);

        remove_null_returns_3 = (remove_binding(name_null, reg_table) == 3);
        if (!remove_null_returns_3)
            printf("%s FAILED on remove_null_returns_3.\n%s\n", test_name, DELIM);

        remove_empty_returns_3 = (remove_binding(name_empty, reg_table) == 3);
        if (!remove_empty_returns_3)
            printf("%s FAILED on remove_empty_returns_3.\n%s\n", test_name, DELIM);

        good_name_still_bound = (lookup_binding(name1, reg_table) == wrapper_ptr);
        if (!good_name_still_bound)
            printf("%s FAILED on good_name_still_bound.\n%s\n", test_name, DELIM);
    }

    destroy_reg_table(reg_table);

    if (init_ok && add_ok && remove_null_returns_3 && remove_empty_returns_3 &&
        good_name_still_bound)
    {
        printf("%s PASSED.\n%s\n", test_name, DELIM);
        return 0;
    }
    else
    {
        return 1;
    }
}

int test_remove_binding_invalid_table()
{
    const char* test_name = "test_remove_binding_invalid_table";
    struct RegistryHash* null_table = NULL;

    bool remove_null_table_returns_3 = false;

    remove_null_table_returns_3 = (remove_binding("A", null_table) == 3);
    if (!remove_null_table_returns_3)
        printf("%s FAILED on remove_null_table_returns_3.\n%s\n", test_name, DELIM);

    if (remove_null_table_returns_3)
    {
        printf("%s PASSED.\n%s\n", test_name, DELIM);
        return 0;
    }
    else
    {
        return 1;
    }
}

int test_destroy_reg_table_null_noop()
{
    const char* test_name = "test_destroy_reg_table_null_noop";
    struct RegistryHash* null_table = NULL;

    bool destroy_null_table_returns_zero = false;

    destroy_null_table_returns_zero = (destroy_reg_table(null_table) == 0);
    if (!destroy_null_table_returns_zero)
        printf("%s FAILED on destroy_null_table_returns_zero.\n%s\n", test_name, DELIM);

    if (destroy_null_table_returns_zero)
    {
        printf("%s PASSED.\n%s\n", test_name, DELIM);
        return 0;
    }
    else
    {
        return 1;
    }
}

int test_destroy_reg_table_nonempty_returns_0()
{
    const char* test_name = "test_destroy_reg_table_nonempty_returns_0";
    const char* name1 = "A";
    const char* name2 = "B";
    struct ObjWrapper* wrapper_ptr = create_scalar(3.14);
    struct RegistryHash* reg_table = init_reg_table(TABLE_SIZE);

    bool init_ok = false;
    bool name1_unbound_before_add1 = false;
    bool add1_ok = false;
    bool name1_bound_after_add1 = false;
    bool name2_unbound_before_add2 = false;
    bool add2_ok = false;
    bool name2_bound_after_add2 = false;
    bool destroy_nonempty_returns_0 = false;

    init_ok = (reg_table != NULL);
    if (!init_ok)
    {
        destroy_reg_table(reg_table);
        printf("%s FAILED on init_ok.\n%s\n", test_name, DELIM);
    }
    else
    {
        name1_unbound_before_add1 = (lookup_binding(name1, reg_table) == NULL);
        if (!name1_unbound_before_add1)
            printf("%s FAILED on name1_unbound_before_add1.\n%s\n", test_name, DELIM);

        add1_ok = (add_binding(name1, wrapper_ptr, reg_table) == 0);
        if (!add1_ok)
            printf("%s FAILED on add1_ok.\n%s\n", test_name, DELIM);

        name1_bound_after_add1 = (lookup_binding(name1, reg_table) == wrapper_ptr);
        if (!name1_bound_after_add1)
            printf("%s FAILED on name1_bound_after_add1.\n%s\n", test_name, DELIM);

        name2_unbound_before_add2 = (lookup_binding(name2, reg_table) == NULL);
        if (!name2_unbound_before_add2)
            printf("%s FAILED on name2_unbound_before_add2.\n%s\n", test_name, DELIM);

        add2_ok = (add_binding(name2, wrapper_ptr, reg_table) == 0);
        if (!add2_ok)
            printf("%s FAILED on add2_ok.\n%s\n", test_name, DELIM);

        name2_bound_after_add2 = (lookup_binding(name2, reg_table) == wrapper_ptr);
        if (!name2_bound_after_add2)
            printf("%s FAILED on name2_bound_after_add2.\n%s\n", test_name, DELIM);

        destroy_nonempty_returns_0 = (destroy_reg_table(reg_table) == 0);
        if (!destroy_nonempty_returns_0)
            printf("%s FAILED on destroy_nonempty_returns_0.\n%s\n", test_name, DELIM);
    }

    if (init_ok && name1_unbound_before_add1 && add1_ok && name1_bound_after_add1 &&
        name2_unbound_before_add2 && add2_ok && name2_bound_after_add2 &&
        destroy_nonempty_returns_0)
    {
        printf("%s PASSED.\n%s\n", test_name, DELIM);
        return 0;
    }
    else
    {
        return 1;
    }
}

int test_lookup_existing_binding()
{
    const char* test_name = "test_lookup_existing_binding";
    const char* name1 = "A";
    const char* name2 = "B";
    struct ObjWrapper* wrapper_ptr = create_scalar(3.14);
    struct RegistryHash* reg_table = init_reg_table(TABLE_SIZE);

    bool init_ok = false;
    bool name1_unbound_before_add1 = false;
    bool add1_ok = false;
    bool name1_bound_after_add1 = false;
    bool name2_unbound_before_add2 = false;
    bool add2_ok = false;
    bool name2_bound_after_add2 = false;
    bool name1_second_lookup_ok = false;
    bool name2_bound_after_name1_lookup = false;
    bool remove_after_lookup_ok = false;
    bool name1_removed_after_lookup = false;
    bool name2_bound_after_remove = false;

    init_ok = (reg_table != NULL);
    if (!init_ok)
        printf("%s FAILED on init_ok.\n%s\n", test_name, DELIM);
    else
    {
        name1_unbound_before_add1 = (lookup_binding(name1, reg_table) == NULL);
        if (!name1_unbound_before_add1)
            printf("%s FAILED on name1_unbound_before_add1.\n%s\n", test_name, DELIM);

        add1_ok = (add_binding(name1, wrapper_ptr, reg_table) == 0);
        if (!add1_ok)
            printf("%s FAILED on add1_ok.\n%s\n", test_name, DELIM);

        name1_bound_after_add1 = (lookup_binding(name1, reg_table) == wrapper_ptr);
        if (!name1_bound_after_add1)
            printf("%s FAILED on name1_bound_after_add1.\n%s\n", test_name, DELIM);

        name2_unbound_before_add2 = (lookup_binding(name2, reg_table) == NULL);
        if (!name2_unbound_before_add2)
            printf("%s FAILED on name2_unbound_before_add2.\n%s\n", test_name, DELIM);

        add2_ok = (add_binding(name2, wrapper_ptr, reg_table) == 0);
        if (!add2_ok)
            printf("%s FAILED on add2_ok.\n%s\n", test_name, DELIM);

        name2_bound_after_add2 = (lookup_binding(name2, reg_table) == wrapper_ptr);
        if (!name2_bound_after_add2)
            printf("%s FAILED on name2_bound_after_add2.\n%s\n", test_name, DELIM);

        name1_second_lookup_ok = (lookup_binding(name1, reg_table) == wrapper_ptr);
        if (!name1_second_lookup_ok)
            printf("%s FAILED on name1_second_lookup_ok.\n%s\n", test_name, DELIM);

        name2_bound_after_name1_lookup = (lookup_binding(name2, reg_table) == wrapper_ptr);
        if (!name2_bound_after_name1_lookup)
            printf("%s FAILED on name2_bound_after_name1_lookup.\n%s\n", test_name, DELIM);

        remove_after_lookup_ok = (remove_binding(name1, reg_table) == 0);
        if (!remove_after_lookup_ok)
            printf("%s FAILED on remove_after_lookup_ok.\n%s\n", test_name, DELIM);

        name1_removed_after_lookup = (lookup_binding(name1, reg_table) == NULL);
        if (!name1_removed_after_lookup)
            printf("%s FAILED on name1_removed_after_lookup.\n%s\n", test_name, DELIM);

        name2_bound_after_remove = (lookup_binding(name2, reg_table) == wrapper_ptr);
        if (!name2_bound_after_remove)
            printf("%s FAILED on name2_bound_after_remove.\n%s\n", test_name, DELIM);
    }

    destroy_reg_table(reg_table);

    if (init_ok && name1_unbound_before_add1 && add1_ok && name1_bound_after_add1 &&
        name2_unbound_before_add2 && add2_ok && name2_bound_after_add2 && name1_second_lookup_ok &&
        name2_bound_after_name1_lookup && remove_after_lookup_ok && name1_removed_after_lookup &&
        name2_bound_after_remove)
    {
        printf("%s PASSED.\n%s\n", test_name, DELIM);
        return 0;
    }
    else
    {
        return 1;
    }
}

int test_lookup_binding_invalid_name()
{
    const char* test_name = "test_lookup_binding_invalid_name";
    const char* name1 = "A";
    const char* name_null = NULL;
    const char* name_empty = "";
    struct ObjWrapper* wrapper_ptr = create_scalar(3.14);
    struct RegistryHash* reg_table = init_reg_table(TABLE_SIZE);

    bool init_ok = false;
    bool add_ok = false;
    bool lookup_null_returns_null = false;
    bool lookup_empty_returns_null = false;
    bool good_name_still_bound = false;

    init_ok = (reg_table != NULL);
    if (!init_ok)
        printf("%s FAILED on init_ok.\n%s\n", test_name, DELIM);
    else
    {
        add_ok = (add_binding(name1, wrapper_ptr, reg_table) == 0);
        if (!add_ok)
            printf("%s FAILED on add_ok.\n%s\n", test_name, DELIM);

        lookup_null_returns_null = (lookup_binding(name_null, reg_table) == NULL);
        if (!lookup_null_returns_null)
            printf("%s FAILED on lookup_null_returns_null.\n%s\n", test_name, DELIM);

        lookup_empty_returns_null = (lookup_binding(name_empty, reg_table) == NULL);
        if (!lookup_empty_returns_null)
            printf("%s FAILED on lookup_empty_returns_null.\n%s\n", test_name, DELIM);

        good_name_still_bound = (lookup_binding(name1, reg_table) == wrapper_ptr);
        if (!good_name_still_bound)
            printf("%s FAILED on good_name_still_bound.\n%s\n", test_name, DELIM);
    }

    destroy_reg_table(reg_table);

    if (init_ok && add_ok && lookup_null_returns_null && lookup_empty_returns_null &&
        good_name_still_bound)
    {
        printf("%s PASSED.\n%s\n", test_name, DELIM);
        return 0;
    }
    else
    {
        return 1;
    }
}

int test_lookup_binding_invalid_table()
{
    const char* test_name = "test_lookup_binding_invalid_table";
    struct RegistryHash* null_table = NULL;

    bool lookup_null_table_returns_null = false;

    lookup_null_table_returns_null = (lookup_binding("A", null_table) == NULL);
    if (!lookup_null_table_returns_null)
        printf("%s FAILED on lookup_null_table_returns_null.\n%s\n", test_name, DELIM);

    if (lookup_null_table_returns_null)
    {
        printf("%s PASSED.\n%s\n", test_name, DELIM);
        return 0;
    }
    else
    {
        return 1;
    }
}

int test_list_bindings()
{
    const char* test_name = "test_list_bindings";
    const char* name1 = "A";
    struct ObjWrapper* wrapper_ptr = create_scalar(3.14);
    struct RegistryHash* reg_table = init_reg_table(TABLE_SIZE);
    struct RegistryHash* null_table = NULL;
    struct RegistryHash* empty_table = init_reg_table(TABLE_SIZE);

    bool init_ok = false;
    bool unbound_before_add = false;
    bool add_ok = false;
    bool bound_after_add = false;
    bool list_bindings_existing_table_returns_0 = false;
    bool bound_after_list_bindings = false;
    bool list_bindings_empty_table_returns_0 = false;
    bool list_bindings_null_table_returns_1 = false;
    bool bound_after_list_null_table = false;

    init_ok = (reg_table != NULL);
    if (!init_ok)
        printf("%s FAILED on init_ok.\n%s\n", test_name, DELIM);
    else
    {
        unbound_before_add = (lookup_binding(name1, reg_table) == NULL);
        if (!unbound_before_add)
            printf("%s FAILED on unbound_before_add.\n%s\n", test_name, DELIM);

        add_ok = (add_binding(name1, wrapper_ptr, reg_table) == 0);
        if (!add_ok)
            printf("%s FAILED on add_ok.\n%s\n", test_name, DELIM);

        bound_after_add = (lookup_binding(name1, reg_table) == wrapper_ptr);
        if (!bound_after_add)
            printf("%s FAILED on bound_after_add.\n%s\n", test_name, DELIM);

        list_bindings_existing_table_returns_0 = (list_bindings(reg_table) == 0);
        if (!list_bindings_existing_table_returns_0)
            printf("%s FAILED on list_bindings_existing_table_returns_0.\n%s\n", test_name, DELIM);

        bound_after_list_bindings = (lookup_binding(name1, reg_table) == wrapper_ptr);
        if (!bound_after_list_bindings)
            printf("%s FAILED on bound_after_list_bindings.\n%s\n", test_name, DELIM);

        list_bindings_empty_table_returns_0 = (list_bindings(empty_table) == 0);
        if (!list_bindings_empty_table_returns_0)
            printf("%s FAILED list_bindings_empty_table_returns_0.\n%s\n", test_name, DELIM);

        list_bindings_null_table_returns_1 = (list_bindings(null_table) == 1);
        if (!list_bindings_null_table_returns_1)
            printf("%s FAILED on list_bindings_null_table_returns_1.\n%s\n", test_name, DELIM);

        bound_after_list_null_table = (lookup_binding(name1, reg_table) == wrapper_ptr);
        if (!bound_after_list_null_table)
            printf("%s FAILED on bound_after_list_null_table.\n%s\n", test_name, DELIM);
    }

    destroy_reg_table(reg_table);

    if (init_ok && unbound_before_add && add_ok && bound_after_add &&
        list_bindings_existing_table_returns_0 && bound_after_list_bindings &&
        list_bindings_empty_table_returns_0 && list_bindings_null_table_returns_1 &&
        bound_after_list_null_table)
    {
        printf("%s PASSED.\n%s\n", test_name, DELIM);
        return 0;
    }
    else
    {
        return 1;
    }
}