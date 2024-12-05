#include "memory.h"

extern sb_memory_t sbMemory(const void * pointer, size_t length)
{
    struct _SbMemory memory = {
            .base = (intptr_t) pointer,
            .length = length
    };
    return memory;
}

extern bool sbMemoryValid(sb_memory_t memory)
{
    if (((void *) memory.base) == nullptr)
    {
        return false;
    }
    if (memory.length == 0)
    {
        return false;
    }
    return true;
}

extern void * sbMemoryOffset(sb_memory_t memory, size_t offset)
{
    if (!sbMemoryValid(memory))
    {
        return nullptr;
    }
    if (offset >= memory.length)
    {
        return nullptr;
    }
    return (void*) (memory.base + offset);
}

extern bool sbMemoryCopy(sb_memory_t origin, sb_memory_t destination)
{
    if (!sbMemoryValid(origin) || !sbMemoryValid(destination))
    {
        return false;
    }
    if (origin.length > destination.length)
    {
        return false;
    }
    size_t i = 0;
    while (i < origin.length)
    {
        *(uint8_t*) (destination.base + i) = *(uint8_t*) (origin.base + i);
        i++;
    }
    return true;
}

