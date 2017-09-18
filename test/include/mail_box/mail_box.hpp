#ifndef MAIL_BOX_MAIL_BOX_HPP
#define MAIL_BOX_MAIL_BOX_HPP

// EXTERNAL DEPENDENCIES
// =============================================================================
#include <utility>  // std::move
#include <iterator> // std::random_access_iterator_tag
#include <atomic>   // std::atomic



template<typename T, std::size_t N>
class MailBox
{
public:
    MailBox()
        : buffer(),
          cursor(&buffer[0])
    {}

    void
    push_back(const T &value)
    {
        push_back(T(value));
    }

    void
    push_back(T &&value)
    {
        T *const next = cursor.fetch_add(1, std::memory_order_relaxed);
        *next         = std::move(value);
    }


private:
    template<typename U>
    class iterator_base
    {
    public:
        typedef std::random_access_iterator_tag iterator_category;
        typedef U                               value_type;
        typedef std::ptrdiff_t                  difference_type;
        typedef U                              *pointer;
        typedef U                              &reference;

        explicit
        iterator_base(U *const ptr) noexcept
            : cursor(ptr)
        {}

        // advancement
        // ---------------------------------------------------------------------
        iterator_base &
        operator++() noexcept
        {
            ++cursor;
            return *this;
        }

        iterator_base
        operator++(int) noexcept
        {
            U *const prev_cursor = cursor++;
            return iterator_base(prev_cursor);
        }

        iterator_base &
        operator+=(const difference_type pos) noexcept
        {
            cursor += pos;
            return *this;
        }

        iterator_base &
        operator--() noexcept
        {
            --cursor;
            return *this;
        }

        iterator_base
        operator--(int) noexcept
        {
            U *const prev_cursor = cursor--;
            return iterator_base(prev_cursor);
        }

        iterator_base &
        operator-=(const difference_type pos) noexcept
        {
            cursor -= pos;
            return *this;
        }

        // comparison
        // ---------------------------------------------------------------------
        friend bool
        operator==(const iterator_base &lhs,
                   const iterator_base &rhs) noexcept
        {
            return lhs.cursor == rhs.cursor;
        }

        friend bool
        operator!=(const iterator_base &lhs,
                   const iterator_base &rhs) noexcept
        {
            return lhs.cursor != rhs.cursor;
        }

        friend bool
        operator<(const iterator_base &lhs,
                  const iterator_base &rhs) noexcept
        {
            return lhs.cursor < rhs.cursor;
        }

        friend bool
        operator>(const iterator_base &lhs,
                  const iterator_base &rhs) noexcept
        {
            return lhs.cursor > rhs.cursor;
        }

        friend bool
        operator<=(const iterator_base &lhs,
                   const iterator_base &rhs) noexcept
        {
            return lhs.cursor <= rhs.cursor;
        }

        friend bool
        operator>=(const iterator_base &lhs,
                   const iterator_base &rhs) noexcept
        {
            return lhs.cursor >= rhs.cursor;
        }

        // distance
        // ---------------------------------------------------------------------
        friend iterator_base
        operator+(const iterator_base  &lhs,
                  const difference_type rhs) noexcept
        {
            return iterator_base(lhs.cursor + rhs);
        }

        friend iterator_base
        operator+(const difference_type lhs,
                  const iterator_base  &rhs) noexcept
        {
            return iterator_base(lhs + rhs.cursor);
        }

        friend difference_type
        operator-(const iterator_base &lhs,
                  const iterator_base &rhs) noexcept
        {
            return lhs.cursor - rhs.cursor;
        }

        // access
        // ---------------------------------------------------------------------
        reference
        operator*() noexcept(noexcept(*cursor))
        {
            return *cursor;
        }

        reference
        operator[](const difference_type pos) noexcept(noexcept(cursor[pos]))
        {
            return cursor[pos];

        }

        pointer
        operator->() const noexcept
        {
            return cursor;
        }


    private:
        U *cursor;
    }; // class iterator_base

public:
    typedef iterator_base<T>       iterator;
    typedef iterator_base<const T> const_iterator;

    iterator
    begin() noexcept
    {
        return iterator(&buffer[0]);
    }

    iterator
    end() noexcept
    {
        return iterator(cursor.load(std::memory_order_relaxed));
    }

    const_iterator
    cbegin() const noexcept
    {
        return const_iterator(&buffer[0]);
    }

    const_iterator
    cend() const noexcept
    {
        return const_iterator(cursor.load(std::memory_order_relaxed));
    }

private:
    T                buffer[N];
    std::atomic<T *> cursor;
}; // class MailBox

#endif // ifndef MAIL_BOX_MAIL_BOX_HPP
