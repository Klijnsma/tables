#ifndef TABLES_H
#define TABLES_H

#include <stddef.h>
#include <stdio.h>

/*
 * Functions for simple table formatting/printing
 * Note that the 'getter' function passed should not 
 * have side effects since it's called multiple times
 */

char* tablefmt(
    const void* data,
    size_t rows,
    size_t columns,
    size_t gap,
    const char* (*getter)(const void* data, size_t x, size_t y)
);

void tableprint(
    const void* data,
    size_t rows,
    size_t columns,
    size_t gap,
    const char* (*getter)(const void* data, size_t x, size_t y),
    FILE* stream
);

#endif
