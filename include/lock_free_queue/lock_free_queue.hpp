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



template<typename T, std::size_t capacity>
class LockFreeQueue
{
static_assert(capacity > 0,
              "capacity must be greater than zero");
private:
    struct Node
    {
        T                   value;
        std::atomic<Node *> next;
    }; // struct Node

    class Allocator
    {
    public:
        Allocator()
            : Allocator(reinterpret_cast<Node *>(&buffer[0]))
        {}

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
        Allocator(Node *next)
            : free(next)
        {
            const Node *const end = next + capacity;

            Node *prev;

            while (true) {
                prev = next++;
                if (next == end)
                    break;

                prev->next.store(next,
                                 std::memory_order_relaxed);
            }

            prev->next.store(nullptr,
                             std::memory_order_release);
        }

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
