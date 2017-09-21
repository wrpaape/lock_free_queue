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



template<typename T>
class LockFreeQueue
{
public:
    LockFreeQueue(const std::size_t capacity)
        : first(allocate(capacity)), // pointer to first element in buffer
          last(first + capacity),    // pointer to last element in buffer
          head(first),
          tail(first)
    {}

    ~LockFreeQueue() noexcept(std::is_nothrow_destructible<T>::value)
    {
        // ensure latest modifications have been loaded
        std::atomic_thread_fence(std::memory_order_acquire);

        destroy<
            std::is_nothrow_destructible<T>::value
        >();
    }

    template<typename ...Args>
    void
    emplace_front(Args &&...args) noexcept(
                                      std::is_nothrow_destructible<T>::value
                                   && std::is_nothrow_constructible<
                                          T,
                                          Args &&...
                                      >::value
                                  )
    {
        // advance head
        T *next_head;
        T *next_tail;

        T *const current_head = head.load(std::memory_order_relaxed);

        if (current_head == last) {
            next_head = first;
            next_tail = next_head + 1;

        } else {
            next_head = current_head + 1;
            next_tail = (next_head == last) ? first : (next_head + 1);
        }

        // copy next_head into temp variable in case CAS fails and resets
        T *overlap = next_head;

        // advance tail if next_head would clobber
        if (tail.compare_exchange_strong(overlap,
                                         next_tail,
                                         std::memory_order_relaxed,
                                         std::memory_order_relaxed))
            overlap->~T(); // make room for next head

        (void) new(current_head) T(std::forward<Args>(args)...);

        // ensure construction writes committed before advancing head
        std::atomic_thread_fence(std::memory_order_release);

        // advance head
        head.store(next_head,
                   std::memory_order_relaxed);
    }


    void
    push_front(const T &value) noexcept(noexcept(emplace_front(value)))
    {
        emplace_front(value);
    }

    void
    push_front(T &&value) noexcept(noexcept(emplace_front(std::move(value))))
    {
        emplace_front(std::move(value));
    }

    bool
    try_pop_back(T &value) noexcept(
                               std::is_nothrow_move_constructible<T>::value
                            && std::is_nothrow_destructible<T>::value
                           )
    {
        return try_pop_back<
            std::is_nothrow_move_constructible<T>::value
         && std::is_nothrow_destructible<T>::value
        >(value);
    }


private:
    template<bool E, typename R = void>
    using enable_if_t = typename std::enable_if<E, R>::type;

    template<bool IsNothrowMoveConstructibleAndDestructible>
    inline enable_if_t<IsNothrowMoveConstructibleAndDestructible, bool>
    try_pop_back(T &value) noexcept
    {
        T *current_tail;

        // acquire tail
        do {
            current_tail = tail.exchange(nullptr,
                                         std::memory_order_relaxed);
        } while (current_tail == nullptr);

        const bool not_empty = (   current_tail
                                != head.load(std::memory_order_relaxed));

        // release tail ASAP
        if (not_empty) {
            // advance tail
            tail.store((current_tail == last) ? first : (current_tail + 1),
                       std::memory_order_relaxed);

            // ensure tail is not accessed mid-construction (from an insertion)
            std::atomic_thread_fence(std::memory_order_acquire);

            value = std::move(*current_tail);

            current_tail->~T();

        } else {
            // replace
            tail.store(current_tail,
                       std::memory_order_relaxed);
        }

        return not_empty;
    }

    template<bool IsNothrowMoveConstructibleAndDestructible>
    inline enable_if_t<!IsNothrowMoveConstructibleAndDestructible, bool>
    try_pop_back(T &value)
    {
        T *current_tail;

        // acquire tail
        do {
            current_tail = tail.exchange(nullptr,
                                         std::memory_order_relaxed);
        } while (current_tail == nullptr);

        T *next_tail = current_tail;

        const bool not_empty = (   current_tail
                                != head.load(std::memory_order_relaxed));

        if (not_empty) {
            // ensure tail is not accessed mid-construction (from an insertion)
            std::atomic_thread_fence(std::memory_order_acquire);

            try {
                value = std::move(*current_tail);

                // advance tail
                next_tail = (current_tail == last) ? first : (current_tail + 1);

                current_tail->~T();

            } catch (...) {
                // advance tail if move succeeded
                tail.store(next_tail,
                           std::memory_order_relaxed);

                throw; // reraise
            }
        }

        // replace tail
        tail.store(next_tail,
                   std::memory_order_relaxed);

        return not_empty;
    }

    inline static T *
    allocate(const std::size_t capacity)
    {
        if (capacity == 0)
            throw std::invalid_argument(
                "LockFreeQueue::allocate() -- capacity must be nonzero"
            );

        T *const buffer = static_cast<T *>(
            std::malloc(sizeof(T) * (capacity + 1))
        );

        if (!buffer)
            throw std::bad_alloc();

        return buffer;
    }

    template<bool IsNothrowDestructible>
    inline enable_if_t<IsNothrowDestructible>
    destroy() noexcept
    {
        T *ptr;

        T *const final_tail = tail.load(std::memory_order_relaxed);
        T *const final_head = head.load(std::memory_order_relaxed);

        if (final_tail <= final_head) {
            for (ptr = final_tail; ptr < final_head; ++ptr)
                ptr->~T();

        } else {
            for (ptr = first; ptr < final_head; ++ptr)
                ptr->~T();

            ptr = final_tail;
            do {
                ptr->~T();
            } while (++ptr <= last);
        }

        std::free(static_cast<void *>(first));
    }

    template<bool IsNothrowDestructible>
    inline enable_if_t<!IsNothrowDestructible>
    destroy()
    {
        try {
            T *ptr;

            T *const final_tail = final_tail.load(std::memory_order_relaxed);
            T *const final_head = head.load(std::memory_order_relaxed);

            if (final_tail <= final_head) {
                for (ptr = final_tail; ptr < final_head; ++ptr)
                    ptr->~T();


            } else {
                for (ptr = first; ptr < final_head; ++ptr)
                    ptr->~T();

                ptr = final_tail;
                do {
                    ptr->~T();
                } while (++ptr <= last);
            }

        } catch (...) {
            // ensure buffer is freed
            std::free(static_cast<void *>(first));
            throw; // reraise
        }

        std::free(static_cast<void *>(first));
    }


    T         *const first;
    T         *const last;
    std::atomic<T *> head;
    std::atomic<T *> tail;
}; // class LockFreeQueue

#endif // ifndef LOCK_FREE_QUEUE_LOCK_FREE_QUEUE_HPP
