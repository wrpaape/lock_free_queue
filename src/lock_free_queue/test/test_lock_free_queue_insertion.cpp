#include <thread>                // std::thread
#include <atomic>                // std::atomic
#include <bitset>                // std::bitset
#include <algorithm>             // std::min
#include <iterator>              // std::distance
#include "mail_box/mail_box.hpp" // MailBox
#include "gtest/gtest.h"         // TEST, ASSERT_*

// LockFreeQueue
#include "lock_free_queue/lock_free_queue.hpp"



TEST(insertion, single_thread)
{
    static const std::size_t mail_box_size = 100;

    for (std::size_t buffer_size = 1000; buffer_size >= 10; buffer_size /= 10) {
        MailBox<int, mail_box_size> mail_box;
        int prev;
        int next;

        LockFreeQueue<int> buffer(buffer_size);

        for (int i = 0; i < mail_box_size; ++i)
            buffer.enqueue(i);

        const std::size_t buffer_count = std::min(mail_box_size, buffer_size);

        for (int i = 0; i < buffer_count; ++i) {
            ASSERT_TRUE(buffer.try_dequeue(next)) << "buffer under-filled";

            mail_box.push_back(next);
        }

        ASSERT_FALSE(buffer.try_dequeue(next)) << "buffer over-filled";

        auto       mail_box_iter = mail_box.cbegin();
        const auto mail_box_end  = mail_box.cend();

        ASSERT_EQ(buffer_count,
                  std::distance(mail_box_iter,
                                mail_box_end)) << "invalid mail_box";

        prev = *mail_box_iter;

        while (++mail_box_iter != mail_box_end) {
            next = *mail_box_iter;

            ASSERT_LT(prev, next) << "insertions out of order";

            prev = next;
        }
    }
}


TEST(insertion, multi_thread)
{
    static const std::size_t count_integers = 1000;

    std::bitset<count_integers>                used_integers;
    MailBox<unsigned int, count_integers * 16> mail_box;
    LockFreeQueue<unsigned int>       buffer(count_integers);
    std::vector<std::thread>                   consumers;

    std::atomic<bool> continue_consuming(true);

    // spawn consumers
    for (unsigned int i = 0; i < 16; ++i)
        consumers.emplace_back([&] {
            unsigned int integer;

            while (continue_consuming)
                if (buffer.try_dequeue(integer))
                    mail_box.push_back(integer);

            while (buffer.try_dequeue(integer))
                mail_box.push_back(integer);
        });

    // write to buffer
    for (unsigned int integer = 0; integer < count_integers; ++integer)
        buffer.enqueue(integer);

    // stop consumers
    continue_consuming = false;

    for (auto &consumer : consumers)
        consumer.join();

    // ensure 'count_integers' unique integers have been written to mail_box
    for (unsigned int integer : mail_box) {
        ASSERT_GT(count_integers,
                  integer) << "invalid read/write";

        std::bitset<count_integers>::reference slot = used_integers[integer];

        ASSERT_FALSE(slot) << "multiple insertions";

        slot = true;
    }

    ASSERT_TRUE(used_integers.all()) << "not all integers accounted for ("
                                     << used_integers.count()
                                     << '/'
                                     << used_integers.size() << ')';
}
