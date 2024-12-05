/**
 * \file memory.h
 *
 * `memory.h` declares Substrate's basic memory manipulation.
 *
 * Memory manipulation is provided through `sb_memory_t` - a fat pointer, and
 * functions to edit, copy, and compare memory using `sb_memory_t`.
 *
 * Substrate is not concerned with memory allocation and management. Memory
 * management is platform-sensitive, and depends on design choice (static vs
 * dynamic allocation.) Substrate lets the user allocate memory as they see
 * fit, before passing blocks of memory to Substrate. The user will also need
 * to deallocate their memory (if relevant) when they've finished using it in
 * Substrate.
 *
 * \author H Paterson.
 * \copyright Boost Software License 1.0.
 */

#ifndef SUBSTRATE_MEMORY_H
#define SUBSTRATE_MEMORY_H

#include <stddef.h>
#include <stdint.h>

/**
 * `_SbMemory` is s fixed-length block of memory.
 *
 * Substrate does not deal with system memory-management. The user must
 * allocate memory before providing it to Substrate, and free it when
 * Substrate has finished using the memory.
 *
 * A `_SbMemory` is a fat-pointer, consisting of an address and length (in
 * bytes.)
 */
struct _SbMemory {
    /** Start address of the memory block. */
    const uintptr_t base;

    /** Size of the block in bytes. */
    const size_t length;
};

typedef struct _SbMemory sb_memory_t;

/**
 * Create a Substrate memory handle.
 *
 * `sbMemory` does not perform sanity checking.
 *
 * \param pointer Memory to encapsulate.
 * \param length Length, in bytes, of the memory addressed by `pointer`.
 */
[[nodiscard("sbMemory is a pure function")]]
sb_memory_t sbMemory(const void* pointer, size_t length);

[[nodiscard("sbMemoryValid is a pure function")]]
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

/**
 * Get a pointer to an offset in the memory.
 *
 * `sbMemoryOffset` generates a pointer to an address in the memory-block. If
 * the offset is 0, the pointer returned will be the base address.
 *
 * \param offset Offset, in bytes, for the pointer into the memory.
 * \return Address to an offset inside the memory.
 */
[[nodiscard("sbMemoryOffset is a pure function")]]
void* sbMemoryOffset(sb_memory_t memory, size_t offset);

/**
 * Copy memory from the origin to the destination.
 *
 * `sbMemoryCopy` will either successfully copy all memory, and return true,
 * or copy no memory and return false, if the operation is not possible.
 *
 * If the origin is longer than the destination, `sbMemoryCopy` will return
 * false and no memory will be copied.
 *
 * @param origin Memory to be copied.
 * @param destination Destination where the origin should be copied.
 */
bool sbMemoryCopy(sb_memory_t origin, sb_memory_t destination);

#endif
