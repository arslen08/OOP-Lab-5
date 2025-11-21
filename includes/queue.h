#pragma once
#include <memory_resource>

template<typename T>
class Queue {
public:
    struct Node {
        T data;
        Node* next;
        Node(const T& value, Node* n = nullptr);
    };

    class Iterator {
    private:
        Node* current;

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        explicit Iterator(Node* node = nullptr);
        reference operator*() const;
        pointer operator->() const;
        Iterator& operator++();
        Iterator operator++(int);
        bool operator==(const Iterator& other) const;
        bool operator!=(const Iterator& other) const;
    };

    using iterator = Iterator;

    Queue(std::pmr::memory_resource* mr = std::pmr::get_default_resource());
    ~Queue();

    void push(const T& value);
    void pop();
    T& front();
    const T& front() const;
    bool empty() const;
    std::size_t size() const;

    iterator begin();
    iterator end();

private:
    Node* head = nullptr;
    Node* tail = nullptr;
    std::size_t count = 0;
    std::pmr::polymorphic_allocator<Node> alloc;
};