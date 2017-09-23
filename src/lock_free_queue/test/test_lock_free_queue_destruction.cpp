#include <stdexcept>     // std::exception
#include "gtest/gtest.h" // TEST, ASSERT_*

// LockFreeQueue
#include "lock_free_queue/lock_free_queue.hpp"



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
        LockFreeQueue<DoNotDestroy>(200)
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
        LockFreeQueue<DtorCounter> buffer(1000);

        for (unsigned int i = 0; i < 1000; ++i)
            buffer.enqueue(count_destroyed);
    }
    ASSERT_EQ(1000,
              count_destroyed) << "count constructions != count destroyed";


    count_destroyed = 0;
    {
        LockFreeQueue<DtorCounter> buffer(100);

        for (unsigned int i = 0; i < 10000; ++i)
            buffer.enqueue(count_destroyed);
    }
    ASSERT_EQ(10000,
              count_destroyed) << "count constructions != count destroyed";
}
