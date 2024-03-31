/**
 * \file memory.h
 *
 * Manage fixed-size blocks of memory.
 *
 * \author H Paterson.
 * \copyright Boost Software License 1.0.
 */

#ifndef SUBSTRATE_MEMORY_H
#define SUBSTRATE_MEMORY_H

#include <stddef.h>
#include <stdint.h>

/**
 * `SbMemory` is s fixed-length block of memory.
 *
 * Substrate does not deal with system memory-management. The user must
 * allocate memory before providing it to Substrate, and free it when
 * Substrate has finished using the memory.
 *
 * A `SbMemory` is a fat-pointer, consisting of an address and length (in
 * bytes.)
 */
struct SbMemory
{
    /** Start address of the memory block. */
    const uintptr_t base;

    /** Size of the block in bytes. */
    const size_t length;
};

typedef struct SbMemory sb_memory_t;

/**
 * Create a Substrate memory handle.
 *
 * `sbMemory` does not perform sanity checking.
 *
 * \param pointer Memory to encapsulate.
 * \param length Length, in bytes, of the memory addressed by `pointer`.
 */
[[nodiscard("Discarding memory handle may cause leaks.")]]
sb_memory_t sbMemory(const void * pointer, size_t length);

[[nodiscard("sbMemoryValid has no side effects")]]
/**
 * Test if a memory object for correctness.
 *
 * `sbMemoryValid` checks a memory object contains a non-null base address, and
 * non-zero length.
 *
 * \param memory Memory to check for correctness.
 * \return True if `memory` is non-null and non-zero length.
 */
bool sbMemoryValid(sb_memory_t memory);


#endif //SUBSTRATE_MEMORY_H
