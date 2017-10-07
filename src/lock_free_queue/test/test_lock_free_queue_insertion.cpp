#include <thread>                // std::thread
#include <atomic>                // std::atomic
#include <bitset>                // std::bitset
#include <algorithm>             // std::min
#include <iterator>              // std::distance
#include "mail_box/mail_box.hpp" // MailBox
#include "gtest/gtest.h"         // TEST, ASSERT_*

// LockFreeQueue
#include "lock_free_queue/lock_free_queue.hpp"


TEST(insertion, capacity)
{
    static const std::size_t capacity = 100;

    LockFreeQueue<int, capacity> queue;

    for (int i = 0; i < capacity; ++i)
        ASSERT_TRUE(queue.try_enqueue(i)) << "capacity reached early";

    ASSERT_FALSE(queue.try_enqueue(0)) << "capacity exceeded";
}
