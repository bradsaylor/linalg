#include "linalg.h"
#include "logs.h"
#include "math_objs.h"
#include "reg_hash.h"

static struct RegistryHash* g_reg_table;

int linalg_create_bind_matrix(struct List elements, size_t num_rows, size_t num_cols,
                              const char* name)
{
    struct ObjWrapper* new_matrix = create_matrix(elements, num_rows, num_cols);
    if (new_matrix == NULL)
        return 2;

    int bind_ret = add_binding(name, new_matrix, g_reg_table);
    if (bind_ret == 0)
        return 0;
    else
    {
        decref_obj(new_matrix);
        switch (bind_ret)
        {
        case 1:
            return 3; // internal error
        case 2:
            return 2; // allocation error
        case 3:
            return 1; // invalid input
        case 5:
            return 3; // internal error
        default:
            return 3; // internal error
        }
    }
}

int linalg_create_bind_vector(struct List elements, const char* name)
{
    struct ObjWrapper* new_vector = create_vector(elements);
    if (new_vector == NULL)
        return 2;

    int bind_ret = add_binding(name, new_vector, g_reg_table);
    if (bind_ret == 0)
        return 0;
    else
    {
        decref_obj(new_vector);
        switch (bind_ret)
        {
        case 1:
            return 3; // internal error
        case 2:
            return 2; // allocation error
        case 3:
            return 1; // invalid input
        case 5:
            return 3; // internal error
        default:
            return 3; // internal error
        }
    }
}

int linalg_create_bind_scalar(double value, const char* name)
{
    struct ObjWrapper* new_scalar = create_scalar(value);
    if (new_scalar == NULL)
        return 2;

    int bind_ret = add_binding(name, new_scalar, g_reg_table);
    if (bind_ret == 0)
        return 0;
    else
    {
        decref_obj(new_scalar);
        switch (bind_ret)
        {
        case 1:
            return 3; // internal error
        case 2:
            return 2; // allocation error
        case 3:
            return 1; // invalid input
        case 5:
            return 3; // internal error
        default:
            return 3; // internal error
        }
    }
}

/* Binding Table API Note:
   g_reg_table is validated by reg_hash APIs;
   callers must initialize via linalg_init_reg_table().
*/

int linalg_init_reg_table(size_t table_size)
{
    g_reg_table = init_reg_table(table_size);
    if (g_reg_table == NULL)
        return 2;
    return 0;
}

int linalg_remove_binding(const char* name)
{
    switch (remove_binding(name, g_reg_table))
    {
    case 0:
        return 0; // success
    case 1:
        return 1; // binding not found-> caller error
    case 3:
        return 1; // invalid input
    case 4:
        return 3; // internal error
    default:
        return 3; // internal error
    }
}

int linalg_shutdown()
{
    destroy_reg_table(g_reg_table);
    g_reg_table = NULL;
    destroy_obj_list();

    return 0;
}
