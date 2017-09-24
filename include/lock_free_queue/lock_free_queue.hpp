#ifndef LOCK_FREE_QUEUE_LOCK_FREE_QUEUE_HPP
#define LOCK_FREE_QUEUE_LOCK_FREE_QUEUE_HPP

// EXTERNAL DEPENDENCIES
// =============================================================================
#include <cstdlib>     // std::[malloc|free]
#include <new>         // std::bad_alloc
#include <stdexcept>   // std::invalid_argument
#include <type_traits> // std::enable_if
#include <utility>     // std::[move|forward]
#include <atomic>      // std::atomic[_thread_fence]



template<typename T, std::size_t Capacity>
class LockFreeQueue
{
private:
    struct Node
    {
        T                   value;
        std::atomic<Node *> next;
    }; // struct Node

    class Allocator
    {
    public:
        Node *
        allocate()
        {
            return nullptr;
        }

        void
        destroy(Node *const node)
        {

        }

    private:
        std::atomic<Node *> free;
        char buffer[sizeof(Node) * capacity];
    }; // class Allocator

public:
    LockFreeQueue()
    {}

    ~LockFreeQueue()
    {}

private:
    std::atomic<Node *> head;
    std::atomic<Node *> tail;
    Allocator           allocator;
}; // class LockFreeQueue

#endif // ifndef LOCK_FREE_QUEUE_LOCK_FREE_QUEUE_HPP
