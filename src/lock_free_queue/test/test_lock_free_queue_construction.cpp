#include <stdexcept>     // std::exception
#include "gtest/gtest.h" // TEST, ASSERT_*

// LockFreeQueue
#include "lock_free_queue/lock_free_queue.hpp"



TEST(construction, no_elements_constructed)
{
    struct DoNotConstruct
    {
        DoNotConstruct()
        {
            throw std::exception();
        }
    }; // struct DoNotConstruct

    ASSERT_NO_THROW(
        LockFreeQueue<DoNotConstruct, 200>()
    ) << "DoNotConstruct() called";
}


TEST(construction, all_elements_constructed)
{
    class CtorCounter
    {
    public:
        CtorCounter(unsigned int &count_constructed)
        {
            ++count_constructed;
        }
    }; // class CtorCounter

    unsigned int count_constructed = 0;

    LockFreeQueue<CtorCounter, 100> buffer;

    for (unsigned int i = 0; i < 100; ++i)
        ASSERT_TRUE(buffer.try_enqueue(count_constructed)) << "out of memory";

    ASSERT_EQ(100,
              count_constructed) << "count_constructed != count inserted";
}
