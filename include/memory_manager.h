#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <memory>
#include <vector>
#include <cstdint>
#include <cstdlib>

class MemoryManager {
private:
    // Pre-allocated memory pools for different object types
    static const size_t SMALL_BLOCK_SIZE = 64;
    static const size_t MEDIUM_BLOCK_SIZE = 1024;
    static const size_t LARGE_BLOCK_SIZE = 16384;
    
    std::vector<uint8_t> small_pool;
    std::vector<uint8_t> medium_pool;
    std::vector<uint8_t> large_pool;
    
    // Free block tracking
    std::vector<void*> small_free_blocks;
    std::vector<void*> medium_free_blocks;
    std::vector<void*> large_free_blocks;
    
    // Memory usage tracking
    size_t total_allocated = 0;
    size_t peak_usage = 0;
    
public:
    bool initialize(size_t total_pool_size = 64 * 1024 * 1024); // 64MB default
    void* allocate(size_t size);
    void deallocate(void* ptr, size_t size);
    size_t get_memory_usage() const;
    size_t get_peak_usage() const;
    void cleanup();
    
private:
    bool is_in_pool(void* ptr, const std::vector<uint8_t>& pool) const;
};

#endif // MEMORY_MANAGER_H