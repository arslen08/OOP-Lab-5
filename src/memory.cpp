#include "../includes/memory.h"
#include <iostream>
#include <algorithm>

Memory::Memory(std::size_t total_size) 
    : total_size(total_size) {
    block = ::operator new(total_size);
    free_blocks[block] = total_size;  
    std::cout << "Memory created with " << total_size << " bytes\n";
}

Memory::~Memory() {
    for (auto& [addr, size] : allocations) {
        std::cout << "Auto-deallocating " << size << " bytes at " << addr << "\n";
    }
    allocations.clear();
    free_blocks.clear();
    ::operator delete(block);
    std::cout << "Memory destroyed\n";
}

void* Memory::find_free_block(std::size_t bytes) {
    for (auto it = free_blocks.begin(); it != free_blocks.end(); ++it) {
        if (it->second >= bytes) {
            return it->first;
        }
    }
    return nullptr;
}

void* Memory::do_allocate(std::size_t bytes, std::size_t) {
    void* free_addr = find_free_block(bytes);
    if (free_addr) {
        std::size_t free_size = free_blocks[free_addr];
        
        allocations[free_addr] = bytes;
        free_blocks.erase(free_addr);
        
        if (free_size > bytes) {
            void* remaining_addr = static_cast<char*>(free_addr) + bytes;
            std::size_t remaining_size = free_size - bytes;
            free_blocks[remaining_addr] = remaining_size;
        }
        
        std::cout << "Allocated " << bytes << " bytes at " << free_addr << "\n";
        return free_addr;
    }
    throw std::bad_alloc();
}

void Memory::do_deallocate(void* p, std::size_t, std::size_t) {
    auto it = allocations.find(p);
    if (it != allocations.end()) {
        std::size_t actual_size = it->second;
        free_blocks[p] = actual_size;  
        allocations.erase(it);
        merge_free_blocks();
        std::cout << "Deallocated " << actual_size << " bytes at " << p << "\n";
    }
}

void Memory::merge_free_blocks() {
    if (free_blocks.empty()) return;
    
    std::vector<std::pair<void*, std::size_t>> sorted_blocks(free_blocks.begin(), free_blocks.end());
    std::sort(sorted_blocks.begin(), sorted_blocks.end(), 
              [](const auto& a, const auto& b) { return a.first < b.first; });
    
    free_blocks.clear();
    if (sorted_blocks.empty()) return;
    
    auto current = sorted_blocks[0];
    for (size_t i = 1; i < sorted_blocks.size(); ++i) {
        void* current_end = static_cast<char*>(current.first) + current.second;
        if (current_end == sorted_blocks[i].first) {
            current.second += sorted_blocks[i].second;
        } else {
            free_blocks[current.first] = current.second;
            current = sorted_blocks[i];
        }
    }

    free_blocks[current.first] = current.second;
}

bool Memory::do_is_equal(const std::pmr::memory_resource& other) const noexcept {
    return this == &other;
}