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
    }

    template<bool IsNothrowMoveConstructibleAndDestructible>
    inline enable_if_t<!IsNothrowMoveConstructibleAndDestructible, bool>
    try_pop_back(T &value)
    {
    }

    template<bool IsNothrowDestructible>
    inline enable_if_t<IsNothrowDestructible>
    destroy() noexcept
    {
    }

    template<bool IsNothrowDestructible>
    inline enable_if_t<!IsNothrowDestructible>
    destroy()
    {
    }
}; // class LockFreeQueue

#endif // ifndef LOCK_FREE_QUEUE_LOCK_FREE_QUEUE_HPP
