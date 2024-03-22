#include "buffer.h"

extern bool sbBufferValid(const struct SbBuffer * const buffer)
{
    if (buffer == nullptr)
    {
        return false;
    }
    if (((void *) buffer->base) == nullptr)
    {
        return false;
    }
    if (buffer->length == 0 || buffer->stride == 0)
    {
        return false;
    }
    if (buffer->length < buffer->stride)
    {
        return false;
    }
    return true;
}

extern size_t sbBufferCapacity(const struct SbBuffer * const buffer)
{
    if (!sbBufferValid(buffer))
    {
        return 0;
    }
    return buffer->length / buffer->stride;
}

extern void * sbBufferIndex(const struct SbBuffer * const buffer, size_t index)
{
    if (!sbBufferValid(buffer))
    {
        return nullptr;
    }
    if (index * buffer->stride >= buffer->length)
    {
        return nullptr;
    }
    return (void *) (index * buffer->stride + buffer->base);
}
