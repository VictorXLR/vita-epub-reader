#include "memory_manager.h"
#include <algorithm>
#include <iostream>

bool MemoryManager::initialize(size_t total_pool_size) {
    // Allocate memory pools at startup
    size_t pool_size_each = total_pool_size / 3;
    
    try {
        small_pool.resize(pool_size_each);
        medium_pool.resize(pool_size_each);
        large_pool.resize(pool_size_each);
    } catch (const std::bad_alloc& e) {
        std::cerr << "Failed to allocate memory pools: " << e.what() << std::endl;
        return false;
    }
    
    // Initialize free block lists
    for (size_t i = 0; i < pool_size_each; i += SMALL_BLOCK_SIZE) {
        if (i + SMALL_BLOCK_SIZE <= pool_size_each) {
            small_free_blocks.push_back(&small_pool[i]);
        }
    }
    
    for (size_t i = 0; i < pool_size_each; i += MEDIUM_BLOCK_SIZE) {
        if (i + MEDIUM_BLOCK_SIZE <= pool_size_each) {
            medium_free_blocks.push_back(&medium_pool[i]);
        }
    }
    
    for (size_t i = 0; i < pool_size_each; i += LARGE_BLOCK_SIZE) {
        if (i + LARGE_BLOCK_SIZE <= pool_size_each) {
            large_free_blocks.push_back(&large_pool[i]);
        }
    }
    
    std::cout << "Memory manager initialized with " << total_pool_size / (1024 * 1024) << "MB" << std::endl;
    std::cout << "Small blocks: " << small_free_blocks.size() << std::endl;
    std::cout << "Medium blocks: " << medium_free_blocks.size() << std::endl;
    std::cout << "Large blocks: " << large_free_blocks.size() << std::endl;
    
    return true;
}

void* MemoryManager::allocate(size_t size) {
    if (size <= SMALL_BLOCK_SIZE && !small_free_blocks.empty()) {
        void* ptr = small_free_blocks.back();
        small_free_blocks.pop_back();
        total_allocated += SMALL_BLOCK_SIZE;
        peak_usage = std::max(peak_usage, total_allocated);
        return ptr;
    } else if (size <= MEDIUM_BLOCK_SIZE && !medium_free_blocks.empty()) {
        void* ptr = medium_free_blocks.back();
        medium_free_blocks.pop_back();
        total_allocated += MEDIUM_BLOCK_SIZE;
        peak_usage = std::max(peak_usage, total_allocated);
        return ptr;
    } else if (size <= LARGE_BLOCK_SIZE && !large_free_blocks.empty()) {
        void* ptr = large_free_blocks.back();
        large_free_blocks.pop_back();
        total_allocated += LARGE_BLOCK_SIZE;
        peak_usage = std::max(peak_usage, total_allocated);
        return ptr;
    }
    
    // Fall back to system malloc for very large allocations
    void* ptr = std::malloc(size);
    if (ptr) {
        total_allocated += size;
        peak_usage = std::max(peak_usage, total_allocated);
    }
    return ptr;
}

void MemoryManager::deallocate(void* ptr, size_t size) {
    if (!ptr) return;
    
    // Determine which pool this pointer belongs to
    if (is_in_pool(ptr, small_pool)) {
        small_free_blocks.push_back(ptr);
        total_allocated -= SMALL_BLOCK_SIZE;
    } else if (is_in_pool(ptr, medium_pool)) {
        medium_free_blocks.push_back(ptr);
        total_allocated -= MEDIUM_BLOCK_SIZE;
    } else if (is_in_pool(ptr, large_pool)) {
        large_free_blocks.push_back(ptr);
        total_allocated -= LARGE_BLOCK_SIZE;
    } else {
        // System-allocated memory
        std::free(ptr);
        total_allocated -= size;
    }
}

bool MemoryManager::is_in_pool(void* ptr, const std::vector<uint8_t>& pool) const {
    if (pool.empty()) return false;
    
    uint8_t* pool_start = const_cast<uint8_t*>(&pool[0]);
    uint8_t* pool_end = pool_start + pool.size();
    uint8_t* test_ptr = static_cast<uint8_t*>(ptr);
    
    return test_ptr >= pool_start && test_ptr < pool_end;
}

size_t MemoryManager::get_memory_usage() const {
    return total_allocated;
}

size_t MemoryManager::get_peak_usage() const {
    return peak_usage;
}

void MemoryManager::cleanup() {
    small_pool.clear();
    medium_pool.clear();
    large_pool.clear();
    
    small_free_blocks.clear();
    medium_free_blocks.clear();
    large_free_blocks.clear();
    
    total_allocated = 0;
    peak_usage = 0;
}