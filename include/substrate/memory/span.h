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
 * @param origin Copy the contents of this span.
 * @param destination Write the contents of the `origin` span into this span.
 * @return True if the span can be copied in full, otherwise no data will be
 * copied and returns false.
 */
bool sbCopySpan(SbSpan origin, SbSpan destination);

/**
 * Create a span which provides a view of an existing span.
 *
 * `sbSubSpan` creates a span which provides access to a subset of another
 * span. The subspan will be `len` bytes wide starting from `offset` in the
 * original span.
 *
 * If a subspan's offset and length would cause it to overrun the original
 * span, the subspan length will be silently limited to the end of the original
 * span.
 *
 * @param parent Create a subspan of this span.
 * @param offset Start the subspan `offset` bytes into the original span.
 * @param len Span `len` bytes from the `offset`, unless the subspan would
 * overrun the end of the original.)
 * @return A span which provides access to a subset of the original span's
 * memory.
 */
SbSpan sbSubSpan(SbSpan parent, size_t offset, size_t len);

#endif //SUBSTRATE_SPAN_H