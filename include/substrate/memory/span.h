/**
 * @brief Spans of memory and functions for copying and setting their contents.
 *
 * @author H Paterson
 * @copyright Boost Software License - Version 1.0
 */

#ifndef SUBSTRATE_SPAN_H
#define SUBSTRATE_SPAN_H


#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


struct _SbSpan {
    /** Pointer to the start span in memory. */
    const intptr_t ptr;

    /** Length of the span, in bytes. */
    const size_t len;
};

/**
 * A fat pointer which stores a memory location and length.
 */
typedef struct _SbSpan SbSpan;


/**
 * Create a new span from memory allocated outside Substrate.
 *
 * A span is a contiguous region of memory
 *
 * `sbCreateSpan` is the primary way to provide memory to Substrate. The caller
 * is responsible for ensuring the memory is accessible and remains accessible
 * for the duration it is used by Substrate.
 *
 * @param ptr Beginning of the memory span.
 * @param len Length of the memory span, in bytes.
 * @return Span representing the memory location and length provided.
 */
SbSpan sbCreateSpan(void * ptr, size_t len);


/**
 * Copy data from one memory location to another.
 *
 * `sbSpanCopy` duplicates the contents from an origin span, to a destination
 * span. The origin span will be copied in full, or not at all. If the origin
 * span cannot be copied in full, no data will be copied and `sbSpanCopy()`
 * will return false.
 *
 * @param origin
 * @param destination
 * @return
 */
bool sbCopySpan(SbSpan origin, SbSpan destination);

/**
 *
 * @param parent
 * @param len
 * @param offset
 * @return
 */
SbSpan SbSubSpan(SbSpan parent, size_t len, size_t offset);

#endif //SUBSTRATE_SPAN_H