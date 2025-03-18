#include "zeno_arc.h"

/**
 * Zeno Automatic Reference Counting - Implementation
 * 
 * This file implements the runtime support functions for Zeno's automatic 
 * reference counting system. It provides the infrastructure for tracking
 * object lifetimes and automatically releasing memory when objects are
 * no longer needed.
 */

// Global statistics tracking (if enabled)
#ifdef ZENO_ARC_STATS
typedef struct {
    size_t total_allocations;     // Total number of allocations
    size_t active_allocations;    // Current active allocations
    size_t total_memory;          // Total memory allocated (in bytes)
    size_t active_memory;         // Current active memory (in bytes)
    size_t total_retains;         // Total number of retain operations
    size_t total_releases;        // Total number of release operations
} ZenoRC_Stats;

static ZenoRC_Stats zeno_rc_stats = {0};

// Get current statistics
ZenoRC_Stats ZenoRC_getStats() {
    return zeno_rc_stats;
}

// Print current statistics
void ZenoRC_printStats() {
    printf("Zeno ARC Statistics:\n");
    printf("  Total allocations:  %zu\n", zeno_rc_stats.total_allocations);
    printf("  Active allocations: %zu\n", zeno_rc_stats.active_allocations);
    printf("  Total memory:       %zu bytes\n", zeno_rc_stats.total_memory);
    printf("  Active memory:      %zu bytes\n", zeno_rc_stats.active_memory);
    printf("  Total retains:      %zu\n", zeno_rc_stats.total_retains);
    printf("  Total releases:     %zu\n", zeno_rc_stats.total_releases);
}

// Update allocation statistics
static void update_alloc_stats(size_t size) {
    zeno_rc_stats.total_allocations++;
    zeno_rc_stats.active_allocations++;
    zeno_rc_stats.total_memory += size;
    zeno_rc_stats.active_memory += size;
}

// Update deallocation statistics
static void update_dealloc_stats(size_t size) {
    zeno_rc_stats.active_allocations--;
    zeno_rc_stats.active_memory -= size;
}

// Update retain statistics
static void update_retain_stats() {
    zeno_rc_stats.total_retains++;
}

// Update release statistics
static void update_release_stats() {
    zeno_rc_stats.total_releases++;
}
#endif

// Registry of type deinitializers
typedef struct ZenoRC_TypeDeinit {
    const char* type_name;
    void (*deinit)(void*);
    struct ZenoRC_TypeDeinit* next;
} ZenoRC_TypeDeinit;

static ZenoRC_TypeDeinit* zeno_rc_type_registry = NULL;

// Register a deinitializer for a type
void ZenoRC_registerDeinit(const char* type_name, void (*deinit)(void*)) {
    // Check if type already registered
    ZenoRC_TypeDeinit* entry = zeno_rc_type_registry;
    while (entry) {
        if (strcmp(entry->type_name, type_name) == 0) {
            // Update existing entry
            entry->deinit = deinit;
            return;
        }
        entry = entry->next;
    }
    
    // Create new entry
    ZenoRC_TypeDeinit* new_entry = (ZenoRC_TypeDeinit*)malloc(sizeof(ZenoRC_TypeDeinit));
    if (!new_entry) {
        fprintf(stderr, "ZenoRC: Memory allocation failed for type registry\n");
        return;
    }
    
    new_entry->type_name = strdup(type_name);
    new_entry->deinit = deinit;
    new_entry->next = zeno_rc_type_registry;
    zeno_rc_type_registry = new_entry;
    
    if (ZENO_ARC_DEBUG) {
        printf("ZenoRC: Registered deinitializer for type %s\n", type_name);
    }
}

// Get the deinitializer for a type
void (*ZenoRC_getDeinit(const char* type_name))(void*) {
    ZenoRC_TypeDeinit* entry = zeno_rc_type_registry;
    while (entry) {
        if (strcmp(entry->type_name, type_name) == 0) {
            return entry->deinit;
        }
        entry = entry->next;
    }
    return NULL;
}

// Cleanup the type registry
void ZenoRC_cleanupRegistry() {
    ZenoRC_TypeDeinit* entry = zeno_rc_type_registry;
    while (entry) {
        ZenoRC_TypeDeinit* next = entry->next;
        free((void*)entry->type_name);
        free(entry);
        entry = next;
    }
    zeno_rc_type_registry = NULL;
    
    if (ZENO_ARC_DEBUG) {
        printf("ZenoRC: Type registry cleaned up\n");
    }
}

// Allocate an object with automatic reference counting
void* ZenoRC_allocObject(size_t size, const char* type_name) {
    // Allocate memory for the header and the object
    size_t total_size = sizeof(ZenoRC_Header) + size;
    ZenoRC_Header* header = (ZenoRC_Header*)malloc(total_size);
    if (!header) {
        fprintf(stderr, "ZenoRC: Memory allocation failed for type %s\n", type_name);
        exit(1);
    }
    
    // Initialize header
    header->ref_count = 1;
    header->deinit = ZenoRC_getDeinit(type_name);
    header->size = size;
    header->type_name = strdup(type_name);
    
    // Get pointer to object memory (after header)
    void* object_ptr = (void*)(header + 1);
    
    // Zero-initialize the object memory
    memset(object_ptr, 0, size);
    
#ifdef ZENO_ARC_STATS
    update_alloc_stats(total_size);
#endif
    
    if (ZENO_ARC_DEBUG) {
        printf("ZenoRC: Allocated %s at %p (count: %d)\n", 
               type_name, object_ptr, header->ref_count);
    }
    
    return object_ptr;
}

// Retain an object (increment its reference count)
void ZenoRC_retainObject(void* ptr) {
    if (!ptr) return;
    
    ZenoRC_Header* header = ((ZenoRC_Header*)ptr) - 1;
    header->ref_count++;
    
#ifdef ZENO_ARC_STATS
    update_retain_stats();
#endif
    
    if (ZENO_ARC_DEBUG) {
        printf("ZenoRC: Retained %s at %p (count: %d)\n", 
               header->type_name, ptr, header->ref_count);
    }
}

// Release an object (decrement reference count and free if zero)
void ZenoRC_releaseObject(void* ptr) {
    if (!ptr) return;
    
    ZenoRC_Header* header = ((ZenoRC_Header*)ptr) - 1;
    header->ref_count--;
    
#ifdef ZENO_ARC_STATS
    update_release_stats();
#endif
    
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
        
        size_t total_size = sizeof(ZenoRC_Header) + header->size;
        
#ifdef ZENO_ARC_STATS
        update_dealloc_stats(total_size);
#endif
        
        // Free the type name
        free((void*)header->type_name);
        
        // Free the memory
        free(header);
    } else if (header->ref_count < 0) {
        fprintf(stderr, "ZenoRC: Error - negative reference count for %s at %p (%d)\n", 
                header->type_name, ptr, header->ref_count);
    }
}

// Set a custom deinitializer for a specific object
void ZenoRC_setObjectDeinit(void* ptr, void (*deinit)(void*)) {
    if (!ptr) return;
    
    ZenoRC_Header* header = ((ZenoRC_Header*)ptr) - 1;
    header->deinit = deinit;
    
    if (ZENO_ARC_DEBUG) {
        printf("ZenoRC: Set deinitializer for %s at %p\n", header->type_name, ptr);
    }
}

// Create a new reference counted string
char* ZenoRC_createString(const char* str) {
    if (!str) return NULL;
    
    size_t len = strlen(str) + 1;
    char* new_str = (char*)ZenoRC_allocObject(len, "String");
    
    // Copy the string data
    strcpy(new_str, str);
    
    return new_str;
}

// Create a reference counted array
void* ZenoRC_createArray(size_t elem_size, size_t count, const char* elem_type) {
    size_t total_size = elem_size * count;
    
    // Create a type name for the array
    char type_name[256];
    snprintf(type_name, sizeof(type_name), "Array<%s>", elem_type);
    
    return ZenoRC_allocObject(total_size, type_name);
}

// Initialize the ARC system
void ZenoRC_initialize() {
    if (ZENO_ARC_DEBUG) {
        printf("ZenoRC: Initializing automatic reference counting system\n");
    }
    
    // Register built-in deinitializers
    
    // String doesn't need a custom deinitializer as it's just a char array
    ZenoRC_registerDeinit("String", NULL);
    
#ifdef ZENO_ARC_STATS
    // Reset statistics
    memset(&zeno_rc_stats, 0, sizeof(ZenoRC_Stats));
#endif
}

// Shutdown the ARC system
void ZenoRC_shutdown() {
    if (ZENO_ARC_DEBUG) {
        printf("ZenoRC: Shutting down automatic reference counting system\n");
    }
    
#ifdef ZENO_ARC_STATS
    // Print final statistics
    ZenoRC_printStats();
#endif
    
    // Clean up type registry
    ZenoRC_cleanupRegistry();
}

// Automatic initialization and cleanup
__attribute__((constructor)) static void __zeno_arc_initialize() {
    ZenoRC_initialize();
}

__attribute__((destructor)) static void __zeno_arc_shutdown() {
    ZenoRC_shutdown();
}