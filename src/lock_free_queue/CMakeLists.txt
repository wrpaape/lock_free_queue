set(
    LOCK_FREE_QUEUE_HEADERS
    ${PROJECT_INCLUDE_DIR}/lock_free_queue/lock_free_queue.hpp
)

if(BUILD_TESTING)
    add_subdirectory(test)
endif()
