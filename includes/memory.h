#pragma once
#include <memory_resource>
#include <map>
#include <vector>
#include <cstddef>

class Memory : public std::pmr::memory_resource {
private:
    void* block;
    std::size_t total_size;
    std::map<void*, std::size_t> allocations;
    std::map<void*, std::size_t> free_blocks;

public:
    explicit Memory(std::size_t total_size);
    ~Memory();

    Memory(const Memory&) = delete;
    Memory& operator=(const Memory&) = delete;

private:
    void* do_allocate(std::size_t bytes, std::size_t alignment) override;
    void do_deallocate(void* p, std::size_t bytes, std::size_t alignment) override;
    bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override;
    
    void merge_free_blocks();
};