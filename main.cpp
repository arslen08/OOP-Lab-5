#include <iostream>
#include "includes/memory.h"
#include "includes/queue.h"

struct Person {
    std::string name;
    int age;
    
    Person(const std::string& n = "", int a = 0) : name(n), age(a) {}
    
    friend std::ostream& operator<<(std::ostream& os, const Person& p) {
        return os << p.name << " (" << p.age << ")";
    }
};

void demo_int() {
    std::cout << "=== DEMO WITH INT ===" << std::endl;
    Memory mr(1024);
    Queue<int> queue(&mr);

    queue.push(1);
    queue.push(2);
    queue.push(3);

    std::cout << "Queue contents: ";
    for (auto it = queue.begin(); it != queue.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;

    std::cout << "Front: " << queue.front() << std::endl;
    queue.pop();
    std::cout << "After pop - Front: " << queue.front() << std::endl;
    std::cout << "Size: " << queue.size() << std::endl;
}

void demo_struct() {
    std::cout << "\n=== DEMO WITH STRUCT ===" << std::endl;
    Memory mr(2048);
    Queue<Person> queue(&mr);

    queue.push(Person("Alice", 25));
    queue.push(Person("Bob", 30));
    queue.push(Person("Charlie", 35));

    std::cout << "Queue contents: ";
    for (auto it = queue.begin(); it != queue.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;

    std::cout << "Size: " << queue.size() << std::endl;
}

int main() {
    demo_int();
    demo_struct();
    return 0;
}