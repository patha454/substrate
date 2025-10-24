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
