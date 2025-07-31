#include <stdlib.h>
#include "rk_os_mem.h"

int os_malloc(void **memptr, size_t alignment, size_t size) {
    return posix_memalign(memptr, alignment, size);
}

int os_realloc(void *src, void **dst, size_t alignment, size_t size) {
    (void)alignment;
    *dst = realloc(src, size);
    return (*dst) ? (0) : (-1);
}

void os_free(void *ptr) {
    free(ptr);
}
