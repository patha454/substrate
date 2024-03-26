#include "memory.h"

extern struct SbMemory sbMemory(const void * pointer, size_t length)
{
    struct SbMemory memory = {
            .base = (intptr_t) pointer,
            .length = length
    };
    return memory;
}

extern bool sbMemoryValid(const struct SbMemory * const memory)
{
    if (memory == nullptr)
    {
        return false;
    }
    if (((void *) memory->base) == nullptr)
    {
        return false;
    }
    if (memory->length == 0)
    {
        return false;
    }
    return true;
}

