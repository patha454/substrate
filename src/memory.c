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
    bool reverse = destination.base > origin.base && destination.base < origin.base + origin.length;

    uint8_t * source = (uint8_t*) (reverse ? origin.base + origin.length - 1 : origin.base);
    uint8_t * target = (uint8_t*) (reverse ? destination.base + origin.length -1 : destination.base);
    int_fast8_t sense = reverse ? -1 : 1;
    size_t i = 0;
    if (reverse)
    {
        while (i < origin.length)
        {
            *(target - i) = *(source + sense * i);
            i++;
        }
    }
    else
    {
        while (i < origin.length)
        {
            *(target + i) = *(source + sense * i);
            i++;
        }
    }
    return true;
}

