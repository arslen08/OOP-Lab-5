#include "../includes/memory.h"
#include <iostream>
#include <vector>
#include <algorithm>

Memory::Memory(std::size_t total_size) : total_size(total_size) {
    block = ::operator new(total_size);
    free_blocks[block] = total_size;
    std::cout << "Memory pool created: " << total_size << " bytes\n";
}

Memory::~Memory() {
    if (!allocations.empty()) {
        std::cout << "Cleaning up " << allocations.size() << " allocations\n";
        allocations.clear();
    }
    free_blocks.clear();
    ::operator delete(block);
    std::cout << "Memory pool destroyed\n";
}

void* Memory::do_allocate(std::size_t bytes, std::size_t) {
    for (const auto& block : free_blocks) {
        if (block.second >= bytes) {
            void* addr = block.first;
            std::size_t size = block.second;
            
            free_blocks.erase(addr);
            allocations[addr] = bytes;
            
            if (size > bytes) {
                void* remaining = static_cast<char*>(addr) + bytes;
                free_blocks[remaining] = size - bytes;
            }
            
            std::cout << "Allocated: " << bytes << " bytes\n";
            return addr;
        }
    }
    throw std::bad_alloc();
}

void Memory::do_deallocate(void* p, std::size_t, std::size_t) {
    auto it = allocations.find(p);
    if (it != allocations.end()) {
        std::size_t size = it->second;
        free_blocks[p] = size;
        allocations.erase(it);
        
        merge_free_blocks();  
        std::cout << "Freed: " << size << " bytes\n";
    }
}

void Memory::merge_free_blocks() {
    if (free_blocks.size() < 2) return;
    
    std::vector<std::pair<void*, std::size_t>> blocks;
    for (const auto& b : free_blocks) {
        blocks.push_back(b);
    }
    
    std::sort(blocks.begin(), blocks.end(), 
        [](const auto& a, const auto& b) { return a.first < b.first; });
    
    free_blocks.clear();
    auto current = blocks[0];
    
    for (size_t i = 1; i < blocks.size(); i++) {
        void* current_end = static_cast<char*>(current.first) + current.second;
        
        if (current_end == blocks[i].first) {
            current.second += blocks[i].second;
        } else {
            free_blocks[current.first] = current.second;
            current = blocks[i];
        }
    }
    free_blocks[current.first] = current.second;
}

bool Memory::do_is_equal(const std::pmr::memory_resource& other) const noexcept {
    return this == &other;
}