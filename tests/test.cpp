#include <gtest/gtest.h>
#include <memory>
#include "memory.h"
#include "queue.h"

struct TestStruct {
    int a;
    double b;
    TestStruct(int a, double b) : a(a), b(b) {}
    bool operator==(const TestStruct& other) const {
        return a == other.a && b == other.b;
    }
};

TEST(MemoryTest, ConstructorAndDestructor) {
    Memory mem(1024);
    std::pmr::polymorphic_allocator<char> alloc(&mem);
    char* ptr = alloc.allocate(100);
    EXPECT_NE(ptr, nullptr);
    alloc.deallocate(ptr, 100);
}

TEST(MemoryTest, AllocateMemory) {
    Memory mem(1024);
    std::pmr::polymorphic_allocator<char> alloc(&mem);
    char* ptr = alloc.allocate(100);
    EXPECT_NE(ptr, nullptr);
    alloc.deallocate(ptr, 100);
}

TEST(MemoryTest, AllocateTooMuchMemory) {
    Memory mem(100);
    std::pmr::polymorphic_allocator<char> alloc(&mem);
    char* ptr = nullptr;
    EXPECT_THROW(ptr = alloc.allocate(200), std::bad_alloc);
    EXPECT_EQ(ptr, nullptr);
}

TEST(MemoryTest, DeallocateMemory) {
    Memory mem(1024);
    std::pmr::polymorphic_allocator<char> alloc(&mem);
    char* ptr = alloc.allocate(100);
    EXPECT_NO_THROW(alloc.deallocate(ptr, 100));
}

TEST(MemoryTest, ReuseMemory) {
    Memory mem(1024);
    std::pmr::polymorphic_allocator<char> alloc(&mem);
    
    char* ptr1 = alloc.allocate(100);
    alloc.deallocate(ptr1, 100);
    
    char* ptr2 = alloc.allocate(100);
    EXPECT_NE(ptr2, nullptr);
    alloc.deallocate(ptr2, 100);
}

TEST(MemoryTest, MultipleAllocations) {
    Memory mem(1024);
    std::pmr::polymorphic_allocator<char> alloc(&mem);
    
    char* ptr1 = alloc.allocate(100);
    char* ptr2 = alloc.allocate(200);
    char* ptr3 = alloc.allocate(300);
    
    EXPECT_NE(ptr1, nullptr);
    EXPECT_NE(ptr2, nullptr);
    EXPECT_NE(ptr3, nullptr);
    
    EXPECT_NE(ptr1, ptr2);
    EXPECT_NE(ptr2, ptr3);
    
    alloc.deallocate(ptr1, 100);
    alloc.deallocate(ptr2, 200);
    alloc.deallocate(ptr3, 300);
}

TEST(MemoryTest, IsEqual) {
    Memory mem1(1024);
    Memory mem2(1024);
    
    EXPECT_TRUE(mem1 == mem1);
    EXPECT_FALSE(mem1 == mem2);
}

TEST(QueueTest, DefaultConstructor) {
    Queue<int> queue;
    EXPECT_TRUE(queue.empty());
    EXPECT_EQ(queue.size(), 0);
}

TEST(QueueTest, PushAndSize) {
    Memory mem(1024);
    Queue<int> queue(&mem);
    
    queue.push(1);
    queue.push(2);
    queue.push(3);
    
    EXPECT_EQ(queue.size(), 3);
    EXPECT_FALSE(queue.empty());
}

TEST(QueueTest, FrontAndPop) {
    Memory mem(1024);
    Queue<int> queue(&mem);
    
    queue.push(10);
    queue.push(20);
    
    EXPECT_EQ(queue.front(), 10);
    queue.pop();
    EXPECT_EQ(queue.front(), 20);
    queue.pop();
    EXPECT_TRUE(queue.empty());
}

TEST(QueueTest, PopEmptyQueue) {
    Memory mem(1024);
    Queue<int> queue(&mem);
    
    EXPECT_NO_THROW(queue.pop());
    EXPECT_TRUE(queue.empty());
}

TEST(QueueTest, Iterator) {
    Memory mem(1024);
    Queue<int> queue(&mem);
    
    queue.push(1);
    queue.push(2);
    queue.push(3);
    
    int sum = 0;
    for (auto it = queue.begin(); it != queue.end(); ++it) {
        sum += *it;
    }
    
    EXPECT_EQ(sum, 6);
}

TEST(QueueTest, IteratorEmpty) {
    Memory mem(1024);
    Queue<int> queue(&mem);
    
    EXPECT_EQ(queue.begin(), queue.end());
}

TEST(QueueTest, IteratorOperators) {
    Memory mem(1024);
    Queue<int> queue(&mem);
    
    queue.push(1);
    queue.push(2);
    
    auto it1 = queue.begin();
    auto it2 = queue.begin();
    
    EXPECT_EQ(it1, it2);
    EXPECT_EQ(*it1, 1);
    
    ++it1;
    EXPECT_NE(it1, it2);
    EXPECT_EQ(*it1, 2);
    
    auto it3 = it1++;
    EXPECT_EQ(*it3, 2);
}

TEST(QueueTest, ComplexType) {
    Memory mem(2048);
    Queue<std::string> queue(&mem);
    
    queue.push("hello");
    queue.push("world");
    queue.push("test");
    
    EXPECT_EQ(queue.size(), 3);
    EXPECT_EQ(queue.front(), "hello");
    
    queue.pop();
    EXPECT_EQ(queue.front(), "world");
}

TEST(QueueTest, StructType) {
    Memory mem(2048);
    Queue<TestStruct> queue(&mem);
    
    queue.push(TestStruct(1, 2.5));
    queue.push(TestStruct(3, 4.7));
    
    EXPECT_EQ(queue.size(), 2);
    EXPECT_TRUE(queue.front() == TestStruct(1, 2.5));
}

TEST(QueueTest, MemoryReuseInQueue) {
    Memory mem(1024);
    Queue<int> queue(&mem);
    
    for (int i = 0; i < 5; ++i) {
        queue.push(i);
    }
    
    while (!queue.empty()) {
        queue.pop();
    }
    
    for (int i = 10; i < 15; ++i) {
        queue.push(i);
    }
    
    EXPECT_EQ(queue.size(), 5);
    EXPECT_EQ(queue.front(), 10);
}

TEST(IntegrationTest, QueueWithCustomMemory) {
    Memory mem(2048);
    Queue<int> queue(&mem);
    
    for (int i = 0; i < 10; ++i) {
        queue.push(i);
    }
    
    EXPECT_EQ(queue.size(), 10);
    
    int sum = 0;
    for (auto it = queue.begin(); it != queue.end(); ++it) {
        sum += *it;
    }
    EXPECT_EQ(sum, 45); 
    
    for (int i = 0; i < 5; ++i) {
        queue.pop();
    }
    EXPECT_EQ(queue.size(), 5);
    EXPECT_EQ(queue.front(), 5);
    
    queue.push(100);
    EXPECT_EQ(queue.size(), 6);
}

TEST(IntegrationTest, MultipleQueuesSameMemory) {
    Memory mem(4096);
    
    Queue<int> queue1(&mem);
    Queue<std::string> queue2(&mem);
    
    queue1.push(42);
    queue2.push("test");
    
    EXPECT_EQ(queue1.front(), 42);
    EXPECT_EQ(queue2.front(), "test");
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}