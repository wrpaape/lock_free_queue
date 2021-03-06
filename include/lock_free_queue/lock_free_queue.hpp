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
              "'capacity' must be greater than zero");
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
            } while (!free.compare_exchange_weak(head,
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
          tail_ptr(nullptr)
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


        std::atomic<Node *> *const
        last_tail_ptr = tail_ptr.exchange(&node->next,
                                          std::memory_order_release);

        if (last_tail_ptr)
            last_tail_ptr->store(node,
                                 std::memory_order_release);
        else
            head.store(node,
                       std::memory_order_release);

        return true;
    }

    bool
    try_dequeue(T &value)
    {
        Node *node;
        Node *next;

        node = head.load(std::memory_order_acquire);

        do {
            if (!node)
                return false;

            next = node->next.load(std::memory_order_acquire);
        } while (!compare_exchange_weak(node,
                                        next,
                                        std::memory_order_acquire,
                                        std::memory_order_acquire));

        value = std::move(node->value);

        node_manager.destroy(node);

        return true;
    }


private:
    std::atomic<Node *>                 head;
    std::atomic< std::atomic<Node *> *> tail_ptr;
    NodeManager                         node_manager;
}; // class LockFreeQueue

#endif // ifndef LOCK_FREE_QUEUE_LOCK_FREE_QUEUE_HPP
