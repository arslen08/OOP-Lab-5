#include "../includes/queue.h"
#include <iostream>
#include <memory> 

struct Person {
    std::string name;
    int age;
    
    Person(const std::string& n = "", int a = 0) : name(n), age(a) {}
    
    friend std::ostream& operator<<(std::ostream& os, const Person& p) {
        return os << p.name << " (" << p.age << ")";
    }
};

struct TestStruct {
    int a;
    double b;
    TestStruct(int a, double b) : a(a), b(b) {}
    bool operator==(const TestStruct& other) const {
        return a == other.a && b == other.b;
    }
};

template<typename T>
Queue<T>::Node::Node(const T& value, Node* n) : data(value), next(n) {}

template<typename T>
Queue<T>::Iterator::Iterator(Node* node) : current(node) {}

template<typename T>
typename Queue<T>::Iterator::reference Queue<T>::Iterator::operator*() const {
    return current->data;
}

template<typename T>
typename Queue<T>::Iterator::pointer Queue<T>::Iterator::operator->() const {
    return &current->data;
}

template<typename T>
typename Queue<T>::Iterator& Queue<T>::Iterator::operator++() {
    if (current) {
        current = current->next;
    }
    return *this;
}

template<typename T>
typename Queue<T>::Iterator Queue<T>::Iterator::operator++(int) {
    Iterator tmp = *this;
    ++(*this);
    return tmp;
}

template<typename T>
bool Queue<T>::Iterator::operator==(const Iterator& other) const {
    return current == other.current;
}

template<typename T>
bool Queue<T>::Iterator::operator!=(const Iterator& other) const {
    return current != other.current;
}

template<typename T>
Queue<T>::Queue(std::pmr::memory_resource* mr) : alloc(mr) {}

template<typename T>
Queue<T>::~Queue() {
    while (!empty()) {
        pop();
    }
}

template<typename T>
void Queue<T>::push(const T& value) {
    Node* new_node = alloc.allocate(1);
    alloc.construct(new_node, value, nullptr);
    
    if (!head) {
        head = tail = new_node;
    } else {
        tail->next = new_node;
        tail = new_node;
    }
    ++count;
}

template<typename T>
void Queue<T>::pop() {
    if (!head) return;
    
    Node* temp = head;
    head = head->next;
    
    std::allocator_traits<decltype(alloc)>::destroy(alloc, temp);
    alloc.deallocate(temp, 1);
    
    --count;
    if (!head) tail = nullptr;
}

template<typename T>
T& Queue<T>::front() {
    return head->data;
}

template<typename T>
const T& Queue<T>::front() const {
    return head->data;
}

template<typename T>
bool Queue<T>::empty() const {
    return head == nullptr;
}

template<typename T>
std::size_t Queue<T>::size() const {
    return count;
}

template<typename T>
typename Queue<T>::iterator Queue<T>::begin() {
    return iterator(head);
}

template<typename T>
typename Queue<T>::iterator Queue<T>::end() {
    return iterator(nullptr);
}

template class Queue<int>;
template class Queue<Person>;
template class Queue<std::string>;
template class Queue<TestStruct>;