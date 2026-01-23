#ifndef LINALG_TYPES_H
#define LINALG_TYPES_H

struct List
{
    void* list;
    size_t size;
    size_t type_size;
};

struct ObjWrapper;

#endif // LINALG_TYPES_H