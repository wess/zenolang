#ifndef ZENO_ARC_H
#define ZENO_ARC_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

/**
 * Zeno Automatic Reference Counting
 * 
 * This header provides the runtime support for automatic reference counting in Zeno.
 * It works by wrapping allocated objects with a header that tracks reference count
 * and custom deinitializers.
 */

// Header structure for reference counted objects
typedef struct {
    int ref_count;                // Number of references to this object
    void (*deinit)(void*);        // Custom deinitializer function
    size_t size;                  // Size of the managed object
    const char* type_name;        // Type name for debugging
} ZenoRC_Header;

// Debug setting - set to true to enable debug messages
#ifndef ZENO_ARC_DEBUG
#define ZENO_ARC_DEBUG false
#endif

// Allocate reference counted memory
static inline void* ZenoRC_alloc(size_t size, const char* type_name) {
    ZenoRC_Header* header = (ZenoRC_Header*)malloc(sizeof(ZenoRC_Header) + size);
    if (!header) {
        fprintf(stderr, "ZenoRC: Memory allocation failed for type %s\n", type_name);
        exit(1);
    }
    
    header->ref_count = 1;
    header->deinit = NULL;
    header->size = size;
    header->type_name = type_name;
    
    void* ptr = (void*)(header + 1);
    
    if (ZENO_ARC_DEBUG) {
        printf("ZenoRC: Allocated %s at %p (count: %d)\n", 
               type_name, ptr, header->ref_count);
    }
    
    // Return pointer to the memory after the header
    return ptr;
}

// Retain (increment reference count)
static inline void ZenoRC_retain(void* ptr) {
    if (!ptr) return;
    
    ZenoRC_Header* header = ((ZenoRC_Header*)ptr) - 1;
    header->ref_count++;
    
    if (ZENO_ARC_DEBUG) {
        printf("ZenoRC: Retained %s at %p (count: %d)\n", 
               header->type_name, ptr, header->ref_count);
    }
}

// Release (decrement reference count and free if zero)
static inline void ZenoRC_release(void* ptr) {
    if (!ptr) return;
    
    ZenoRC_Header* header = ((ZenoRC_Header*)ptr) - 1;
    header->ref_count--;
    
    if (ZENO_ARC_DEBUG) {
        printf("ZenoRC: Released %s at %p (count: %d)\n", 
               header->type_name, ptr, header->ref_count);
    }
    
    if (header->ref_count == 0) {
        if (ZENO_ARC_DEBUG) {
            printf("ZenoRC: Deallocating %s at %p\n", header->type_name, ptr);
        }
        
        // Call custom destructor if provided
        if (header->deinit) {
            header->deinit(ptr);
        }
        
        // Free the memory
        free(header);
    } else if (header->ref_count < 0) {
        fprintf(stderr, "ZenoRC: Error - negative reference count for %s at %p (%d)\n", 
                header->type_name, ptr, header->ref_count);
    }
}

// Set custom destructor for an object
static inline void ZenoRC_setDeinit(void* ptr, void (*deinit)(void*)) {
    if (!ptr) return;
    
    ZenoRC_Header* header = ((ZenoRC_Header*)ptr) - 1;
    header->deinit = deinit;
    
    if (ZENO_ARC_DEBUG) {
        printf("ZenoRC: Set deinitializer for %s at %p\n", header->type_name, ptr);
    }
}

// Get reference count (for debugging)
static inline int ZenoRC_getCount(void* ptr) {
    if (!ptr) return 0;
    
    ZenoRC_Header* header = ((ZenoRC_Header*)ptr) - 1;
    return header->ref_count;
}

// Create a copy of an object (with new reference count)
static inline void* ZenoRC_copy(void* ptr) {
    if (!ptr) return NULL;
    
    ZenoRC_Header* header = ((ZenoRC_Header*)ptr) - 1;
    void* new_ptr = ZenoRC_alloc(header->size, header->type_name);
    
    // Copy the data
    memcpy(new_ptr, ptr, header->size);
    
    // Copy the destructor
    ZenoRC_setDeinit(new_ptr, header->deinit);
    
    if (ZENO_ARC_DEBUG) {
        printf("ZenoRC: Created copy of %s from %p to %p\n", 
               header->type_name, ptr, new_ptr);
    }
    
    return new_ptr;
}

// Create a string (with reference counting)
static inline char* ZenoRC_string(const char* str) {
    if (!str) return NULL;
    
    size_t len = strlen(str) + 1;
    char* new_str = (char*)ZenoRC_alloc(len, "String");
    
    // Copy the string data
    strcpy(new_str, str);
    
    return new_str;
}

// Get the type name of an object
static inline const char* ZenoRC_typeName(void* ptr) {
    if (!ptr) return "NULL";
    
    ZenoRC_Header* header = ((ZenoRC_Header*)ptr) - 1;
    return header->type_name;
}

// Dump reference counting info for debugging
static inline void ZenoRC_dump(void* ptr) {
    if (!ptr) {
        printf("ZenoRC: NULL pointer\n");
        return;
    }
    
    ZenoRC_Header* header = ((ZenoRC_Header*)ptr) - 1;
    printf("ZenoRC Object:\n");
    printf("  Address:      %p\n", ptr);
    printf("  Type:         %s\n", header->type_name);
    printf("  Size:         %zu bytes\n", header->size);
    printf("  Ref Count:    %d\n", header->ref_count);
    printf("  Deinitializer: %s\n", header->deinit ? "Yes" : "No");
}

// Convenience macros for common ARC operations
#define ZENO_ALLOC(type) ((type*)ZenoRC_alloc(sizeof(type), #type))
#define ZENO_RETAIN(ptr) ZenoRC_retain(ptr)
#define ZENO_RELEASE(ptr) ZenoRC_release(ptr)
#define ZENO_STRING(str) ZenoRC_string(str)

#endif // ZENO_ARC_H