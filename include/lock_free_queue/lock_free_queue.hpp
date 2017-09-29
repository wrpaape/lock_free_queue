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

    class NodeManager
    {
    public:
        NodeManager()
            : NodeManager(reinterpret_cast<Node *>(&buffer[0]))
        {}

        template<typename ...Args>
        Node *
        construct(Args &&...args)
        {
            Node *node;

            node = free.load(std::memory_order_acquire);
            do {
                if (!node)
                    return node;

                Node *const next = node->next.load(std::memory_order_acquire);
            } while (!free.compare_exchange_weak(node,
                                                 next,
                                                 std::memory_order_acquire,
                                                 std::memory_order_acquire));

            (void) new(&node->value) T(std::forward<Args>(args)...);
            return node;
        }

        void
        destroy(Node *const node)
        {
            node->~T();

            std::atomic<Node *> &node_next = node->next;
            Node *head = free.load(std::memory_order_acquire);

            do {
                // link node to current head of free stack
                node_next.store(head,
                                std::memory_order_acquire);
                // swap node into 'free' as new head
            } while (free.compare_exchange_weak(head,
                                                node,
                                                std::memory_order_acquire,
                                                std::memory_order_acquire));
        }

    private:
        NodeManager(Node *next)
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
    }; // class NodeManager

public:
    LockFreeQueue()
        : node_manager(),
          head(nullptr),
          tail(nullptr)
    {}

    ~LockFreeQueue()
    {}

    template<typename ...Args>
    bool
    try_enqueue(Args &&...args)
    {
        Node *const node = node_manager.construct(std::forward<Args>(args)...);

        if (!node)
            return false;

    }


private:
    std::atomic<Node *> head;
    std::atomic<Node *> tail;
    NodeManager         node_manager;
}; // class LockFreeQueue

#endif // ifndef LOCK_FREE_QUEUE_LOCK_FREE_QUEUE_HPP
