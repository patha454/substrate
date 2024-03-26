#include "memory.h"

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

