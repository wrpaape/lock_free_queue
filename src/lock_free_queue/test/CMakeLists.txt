set(additional_insertion_sources
    ${MAIL_BOX_HEADERS})

foreach(component construction destruction insertion)
    add_google_test(
        NAME    test_lock_free_queue_${component}
        SOURCES test_lock_free_queue_${component}.cpp
                ${LOCK_FREE_QUEUE_HEADERS}
                ${additional_${component}_sources}
    )
endforeach()
