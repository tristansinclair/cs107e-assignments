/*
 * File: malloc.c
 * --------------
 * This is the simple "bump" allocator from lecture.
 * An allocation request is serviced by using sbrk to extend
 * the heap segment. 
 * It does not recycle memory (free is a no-op) so when all the
 * space set aside for the heap is consumed, it will not be able
 * to service any further requests.
 *
 * This code is given here just to show the very simplest of
 * approaches to dynamic allocation. You are to replace this code
 * with your own heap allocator implementation.
 */

#include "malloc.h"
#include "malloc_internal.h"
#include "printf.h"
#include <stddef.h> // for NULL
#include "strings.h"

extern int __bss_end__;

#define STACK_START 0x8000000
#define STACK_SIZE 0x1000000
#define STACK_END ((char *)STACK_START - STACK_SIZE)

/*
 * The pool of memory available for the heap starts at the upper end of the 
 * data section and can extend from there up to the lower end of the stack.
 * It uses symbol __bss_end__ from memmap to locate data end
 * and calculates stack end assuming a 16MB stack.
 *
 * Global variables for the bump allocator:
 *
 * `heap_start`  location where heap segment starts
 * `heap_end`    location at end of in-use portion of heap segment 
 */

// Initial heap segment starts at bss_end and is empty
static void *heap_start = &__bss_end__;
static void *heap_end = &__bss_end__;

void *sbrk(int nbytes)
{
    void *prev_end = heap_end;
    if ((char *)prev_end + nbytes > STACK_END)
    {
        return NULL;
    }
    else
    {
        heap_end = (char *)prev_end + nbytes;
        return prev_end;
    }
}

// Simple macro to round up x to multiple of n.
// The efficient but tricky bitwise approach it uses
// works only if n is a power of two -- why?
#define roundup(x, n) (((x) + ((n)-1)) & (~((n)-1)))

struct header
{
    unsigned int size;   // 4 bytes
    unsigned int status; // 4 bytes
} header;
enum
{
    IN_USE = 0,
    FREE = 1
};

void *malloc(size_t nbytes)
{
    if (nbytes == 0)
    {
        return NULL;
    }

    nbytes = roundup(nbytes, 8); // align on 8 byte system

    struct header *hdr = (struct header *)(heap_start); // points to heap start
    size_t size = hdr->size;                            // size of current block
    int status = hdr->status;                           // status of current block

    // In order to recycle:
    // status == FREE
    // total_bytes <= size
    // hdr < heap_end

    while (status == IN_USE && nbytes > size && (void *)hdr < heap_end) // search for available space
    {
        hdr += 1 + (size / 8);
        size = hdr->size;
        status = hdr->status;
    }

    if ((void *)hdr < heap_end) // if this is a recycled space.... (not at the end of the heap)
    {
        hdr->size = nbytes;   // write new size to hdr
        hdr->status = IN_USE; // set status to IN_USE

        if (size - nbytes >= 8) // if there is enough space for a header or more (not a perfect match)
        {
            struct header *new_hdr = hdr + 1 + (nbytes / 8);
            new_hdr->size = size - nbytes - 8; // size - new size - new hdr
            free((void *)(new_hdr + 1));       // might as well check to clear up some more space here
            //new_hdr->status = FREE;
        }
        return hdr + 1;
    }

    // If we didn't find a recycled space, add to the end of the heap
    size_t total_bytes = nbytes + 8;

    int *prev_end = (int *)sbrk(total_bytes);
    if (prev_end == NULL) // if heap is full
    {
        return NULL;
    }
    hdr = (struct header *)prev_end; // hdr points to start point of malloc
    hdr->size = nbytes;              // stores bytes requested WITHOUT HEADER SIZE
    hdr->status = IN_USE;
    hdr++;

    void *data_start = (void *)hdr;

    return data_start;
}

void clean_heap()
{
    heap_end = heap_start;
}

void free(void *ptr)
{
    // expects to receive ptr returned from malloc therefore it points to first byte of DATA not the header
    struct header *hdr = ptr;
    hdr--; // moves back to beginning of the header
    hdr->status = FREE;

    size_t size = hdr->size;
    struct header *next_hdr = hdr + 1 + (size / 8);
    size_t next_size = next_hdr->size;
    int next_status = next_hdr->status;

    while (next_status == FREE) //  && (void *)next_hdr < heap_end
    {
        hdr->size += next_size + 8; // update original header size

        // move to next header
        next_hdr += 1 + (next_size / 8);
        next_size = next_hdr->size;
        next_status = next_hdr->status;
    }
}

void *realloc(void *orig_ptr, size_t new_size)
{
    struct header *hdr = orig_ptr; // hdr starts pointing to headers
    //hdr--;
    size_t size = hdr->size;
    int status = hdr->status;

    if (new_size < size && status == FREE)
    {
        hdr->size = new_size;
        hdr->status = IN_USE;

        if (size - new_size >= 16) // if there are at least 16 bytes remaining !!!! MEMORY LEAK IF LESS THAN 16 BYTES REMAIN
        {
            struct header *new_hdr = hdr + 1 + (new_size / 8);
            new_hdr->size = size - new_size - 8; // size - new size - new hdr
            new_hdr->status = FREE;
        }
    }
    return (void *)hdr;

    void *new_ptr = malloc(new_size);
    if (!new_ptr)
        return NULL;

    // ideally would copy the min of new_size and old_size, but this allocator
    // doesn't know the old_size. Why not?
    // Why is it "safe" (but not efficient) to copy new_size bytes?
    memcpy(new_ptr, orig_ptr, new_size);
    free(orig_ptr);
    return new_ptr;
}

void heap_dump(const char *label)
{
    printf("\n---------- HEAP DUMP (%s) ----------\n", label);
    printf("Heap segment at %p - %p\n", heap_start, heap_end);

    struct header *hdr = (struct header *)(heap_start);
    size_t size = hdr->size;
    int status = hdr->status;
    char *str;

    while ((void *)hdr < heap_end)
    {
        str = status == 1 ? "FREE" : "IN_USE";
        printf("Header Location: %p     Block Location: %p     Size: %d     Status: %s\n", hdr, hdr + 1, size, str);

        hdr += 1 + (size / 8); // +1 to move to data, +(size/8) to move to next header
        size = hdr->size;
        status = hdr->status;
    }

    printf("----------  END DUMP (%s) ----------\n", label);
}

void memory_report(void)
{
    printf("\n=============================================\n");
    printf("         Mini-Valgrind Memory Report         \n");
    printf("=============================================\n");
    // TODO: fill in for extension
}
