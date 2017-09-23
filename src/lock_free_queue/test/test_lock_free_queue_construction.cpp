#include <stdexcept>     // std::exception
#include "gtest/gtest.h" // TEST, ASSERT_*

// LockFreeQueue
#include "lock_free_queue/lock_free_queue.hpp"



TEST(construction, capacity_gt_zero)
{
    ASSERT_NO_THROW(
        LockFreeQueue<int>(1)
    ) << "constructor threw with valid capacity";

    ASSERT_THROW(
        LockFreeQueue<int>(0),
        std::invalid_argument
    ) << "constructor succeeded with (invalid) capacity of 0";
}


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
        LockFreeQueue<DoNotConstruct>(200)
    ) << "DoNotConstruct() called";
}


TEST(construction, all_elements_constructed)
{
    unsigned int count_constructed;

    class CtorCounter
    {
    public:
        CtorCounter(unsigned int &count_constructed)
        {
            ++count_constructed;
        }
    }; // class CtorCounter

    count_constructed = 0;

    LockFreeQueue<CtorCounter> buffer(100);

    for (unsigned int i = 0; i < 10; ++i)
            buffer.enqueue(count_constructed);

    ASSERT_EQ(10,
              count_constructed) << "count_constructed != count inserted";

    count_constructed = 0;

    for (unsigned int i = 0; i < 1000; ++i)
            buffer.enqueue(count_constructed);

    ASSERT_EQ(1000,
              count_constructed) << "count_constructed != count inserted";
}
