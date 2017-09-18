set(additional_insertion_sources
    ${MAIL_BOX_HEADERS})

foreach(component construction destruction insertion)
    add_google_test(
        NAME    test_ring_buffer_${component}
        SOURCES test_ring_buffer_${component}.cpp
                ${SINGLE_WRITER_RING_BUFFER_HEADERS}
                ${additional_${component}_sources}
    )
endforeach()
