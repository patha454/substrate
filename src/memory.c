#include "memory.h"

extern sb_memory_t sbMemory(const void* pointer, size_t length)
{
    struct _SbMemory memory = { .base = (intptr_t)pointer, .length = length };
    return memory;
}

extern bool sbMemoryValid(sb_memory_t memory)
{
    if ((void*)memory.base == nullptr) {
        return false;
    }
    if (memory.length == 0) {
        return false;
    }
    return true;
}

extern void* sbMemoryOffset(sb_memory_t memory, size_t offset)
{
    if (!sbMemoryValid(memory)) {
        return nullptr;
    }
    if (offset >= memory.length) {
        return nullptr;
    }
    return (void*)(memory.base + offset);
}

extern bool sbMemoryOverlapLow(const sb_memory_t reference, const sb_memory_t query)
{
    if (!sbMemoryValid(reference) || !sbMemoryValid(query)) {
        return false;
    }
    if (query.base <= reference.base && query.base + query.length > reference.base) {
        return true;
    }
    return false;
}

extern bool sbMemoryOverlapHigh(
    const sb_memory_t reference, const sb_memory_t query)
{
    if (!sbMemoryValid(reference) || !sbMemoryValid(query)) {
        return false;
    }
    if (query.base < reference.base + reference.length && query.base + query.length >= reference.base + reference.length) {
        return true;
    }
    return false;
}

extern bool sbMemoryCopy(sb_memory_t origin, sb_memory_t destination)
{
    if (!sbMemoryValid(origin) || !sbMemoryValid(destination)) {
        return false;
    }
    if (origin.length > destination.length) {
        return false;
    }
    const bool reverse = sbMemoryOverlapHigh(origin, destination);
    auto source
        = (intptr_t)(reverse ? origin.base + origin.length - 1 : origin.base);
    auto target = (intptr_t)(reverse ? destination.base + origin.length - 1
                                     : destination.base);
    const int_fast8_t sense = reverse ? -1 : 1;
    size_t i = 0;

    // Crawl to word alignment...
    while (i < origin.length && (uintptr_t)source % sizeof(uintmax_t) != 0) {
        *(uint8_t*)target = *(uint8_t*)source;
        target += sense;
        source += sense;
        i += 1;
    }

    // Copy words for speed...
    while (i < origin.length && i + sizeof(uintmax_t) <= origin.length) {
        *(uintmax_t*)target = *(uintmax_t*)source;
        target += sense * (uint_fast8_t)sizeof(uintmax_t);
        source += sense * (uint_fast8_t)sizeof(uintmax_t);
        i += sizeof(uintmax_t);
    }

    // Crawl to end of copy...
    while (i < origin.length) {
        *(uint8_t*)target = *(uint8_t*)source;
        target += sense;
        source += sense;
        i += 1;
    }
    return true;
}
