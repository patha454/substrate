/**
 * \file buffer.h
 *
 * \author H Paterson.
 * \copyright Boost Software License 1.0.
 */

#ifndef SUBSTRATE_BUFFER_H
#define SUBSTRATE_BUFFER_H

#include <stddef.h>
#include <stdint.h>

/**
 * `SbBuffer` is s fixed-length buffer in one or more items in memory.
 *
 * A `SbBuffer` is a fat-pointer, consisting of an address, length,
 * and distance between each index in the buffer (stride.)
 */
struct SbBuffer
{
    /** Start address of the buffer's memory slab. */
    const uintptr_t base;

    /** Size of the buffer, in bytes. */
    const size_t length;

    /** Distance, in bytes, between elements in the array. */
    const size_t stride;
};

[[nodiscard]]
/**
 * Test if a buffer is valid for use.
 *
 * Check if a pointer is to a correct `SbBuffer`. The check will failed if
 * the pointer is null, or buffer structure fails sanity checks.
 *
 * \param buffer Buffer to check for correctness.
 * \return True if `buffer` is non-null and sane.
 */
bool sbBufferValid(const struct SbBuffer * buffer);

[[nodiscard]]
/**
 * Calculate the capacity of a buffer, in items.
 *
 * `sbBufferCapacity` returns the number of objects which fit into the buffer.
 * The capacity (in items) is necessarily the same as the length (in bytes).
 *
 * @param buffer Check the capacity of this buffer.
 * @return Capacity of the buffer, or 0 if the buffer is invalid.
 */
size_t sbBufferCapacity(const struct SbBuffer * buffer);

[[nodiscard]]
/**
 * Get a pointer to an object in the buffer.
 *
 * `sbBufferIndex` returns a pointer to the `index`-th idem in the buffer.
 *
 * @param buffer
 * @param index
 * @return Pointer to the item at index, or `nullptr` if `buffer` or `index` are invalid.
 */
void * sbBufferIndex(const struct SbBuffer * buffer, size_t index);

#endif //SUBSTRATE_BUFFER_H
