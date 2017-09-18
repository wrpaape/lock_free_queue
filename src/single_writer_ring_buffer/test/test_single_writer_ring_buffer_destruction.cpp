#include <stdexcept>     // std::exception
#include "gtest/gtest.h" // TEST, ASSERT_*

// RingBuffer
#include "ring_buffer/ring_buffer.hpp"



TEST(destruction, no_elements_destroyed)
{
    struct DoNotDestroy
    {
        ~DoNotDestroy()
        {
            throw std::exception();
        }
    }; // struct DoNotDestroy

    ASSERT_NO_THROW(
        RingBuffer<DoNotDestroy>(200)
    ) << "~DoNotDestroy() called";
}


TEST(destruction, all_elements_destroyed)
{
    unsigned int count_destroyed;

    class DtorCounter
    {
    public:
        DtorCounter(unsigned int &count_destroyed)
            : count_destroyed(count_destroyed)
        {}

        ~DtorCounter()
        {
            ++count_destroyed;
        }
    private:
        unsigned int &count_destroyed;
    }; // class DtorCounter


    count_destroyed = 0;
    {
        RingBuffer<DtorCounter> buffer(1000);

        for (unsigned int i = 0; i < 1000; ++i)
            buffer.emplace_front(count_destroyed);
    }
    ASSERT_EQ(1000,
              count_destroyed) << "count constructions != count destroyed";


    count_destroyed = 0;
    {
        RingBuffer<DtorCounter> buffer(100);

        for (unsigned int i = 0; i < 10000; ++i)
            buffer.emplace_front(count_destroyed);
    }
    ASSERT_EQ(10000,
              count_destroyed) << "count constructions != count destroyed";
}
