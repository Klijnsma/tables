#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>

#ifndef MAX_STACK_COLUMNS
#define MAX_STACK_COLUMNS 1024
#endif

#define max(lhs, rhs) \
    ((lhs) > (rhs) ? (lhs) : (rhs))

// select stack or heap based buffer for column widths based on column count
#define select_buffer(fail_ret)                               \
    do {                                                      \
        if (columns <= MAX_STACK_COLUMNS) {                   \
            col_widths = widths_stack;                        \
        } else {                                              \
            widths_heap = malloc(columns * sizeof(size_t));   \
            if (col_widths == NULL) { return fail_ret; }      \
            col_widths = widths_heap;                         \
        }                                                     \
    }                                                         \
    while (0)

static size_t get_widths(
    const void* data,
    size_t rows,
    size_t columns,
    size_t gap,
    const char* (*getter)(const void* data, size_t x, size_t y),
    size_t* out
) {
    /*
     * populates out with length for each column, and 
     * returns total line length (including newline)
     */
    size_t retval = 0;
    for (size_t r = 0; r < rows; r++) {
        size_t field_length_sum = 0;
        for (size_t c = 0; c < columns; c++) {
            size_t field_length = c < columns - 1
                ? strlen(getter(data, c, r)) + gap
                : strlen(getter(data, c, r)) + 1;
            out[c] = max(out[c], field_length);
            field_length_sum += out[c];
        }
        retval = max(retval, field_length_sum);
    }
    return retval;
}

char* tablefmt(
    const void* data,
    size_t rows,
    size_t columns,
    size_t gap,
    const char* (*getter)(const void* data, size_t x, size_t y)
) {
    size_t widths_stack[MAX_STACK_COLUMNS];
    size_t* widths_heap = NULL;
    size_t* col_widths;

    select_buffer(NULL);

    size_t line_length = get_widths(data, rows, columns, gap, getter, col_widths);
    size_t size = line_length * rows;
    char* retval = malloc(size + 1);
    if (retval == NULL) {
        if (widths_heap != NULL) { free(widths_heap); }
        return NULL;
    }
    memset(retval, ' ', size);
    retval[size] = '\0';

    for (size_t r = 0; r < rows; r++) {
        size_t pos = 0;
        for (size_t c = 0; c < columns; c++) {
            const char* item = getter(data, c, r);
            memcpy(
                retval + ((r * line_length) + pos),
                item,
                strlen(item)
            );
            pos += col_widths[c];
        }
        retval[r * line_length + (pos-1)] = '\n';
    }

    if (widths_heap != NULL) { free(widths_heap); }

    return retval;
}

void tableprint(
    const void* data,
    size_t rows,
    size_t columns,
    size_t gap,
    const char* (*getter)(const void* data, size_t x, size_t y),
    FILE* stream
) {
    size_t widths_stack[MAX_STACK_COLUMNS];
    size_t* widths_heap = NULL;
    size_t* col_widths;

    select_buffer();

    get_widths(data, rows, columns, gap, getter, col_widths);

    for (size_t r = 0; r < rows; r++) {
        size_t pos = 0;
        for (size_t c = 0; c < columns; c++) {
            const char* item = getter(data, c, r);
            fprintf(stream, "\x1b[%zuG%s", pos + 1, item);
            pos += col_widths[c];
        }
        fputc('\n', stream);
    }

    if (widths_heap != NULL) { free(widths_heap); }
}

