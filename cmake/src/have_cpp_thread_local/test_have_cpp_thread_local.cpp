#include <iostream> // std::cout
#include <thread>   // std::thread

static thread_local int counter;

void
routine(const int inc)
{
    std::cout << "count: " << counter << std::endl;
    counter += inc;
    std::cout << "count: " << counter << std::endl;
}

int
main(int argc,
     char *argv[])
{
    while (--argc >= 0) {
        std::thread run_routine(&routine,
                                argc);
        run_routine.join();
    }

    return counter;
}
