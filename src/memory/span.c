#include <assert.h>
#include "substrate/memory/span.h"

extern SbSpan sbCreateSpan(void * ptr, size_t len) {
    if (ptr == nullptr) {
        len = 0;
    }
    struct _SbSpan span = {
        .ptr = (intptr_t) ptr,
        .len = len
    };
    return span;
}

extern bool sbCopySpan(SbSpan origin, SbSpan destination) {
    if (origin.len == 0 || destination.len == 0) {
        return true;
    }
    const size_t copyLen =
        origin.len < destination.len ? origin.len : destination.len;
    /*
     * The copy must be done in the reverse order if the end of the origin
     * overlaps with the start of the destination.
     */
    bool reverse = destination.ptr > origin.ptr &&
        destination.ptr < origin.ptr + copyLen;

    if (reverse) {
        for (size_t i = copyLen; i > 0; i--) {
            ((uint8_t *) destination.ptr)[i - 1] = ((uint8_t *) origin.ptr)[i - 1];
        }
        return true;
    }

    for (size_t i = 0; i < copyLen; i++) {
        ((uint8_t *) destination.ptr)[i] = ((uint8_t *) origin.ptr)[i];
    }

    return true;
}