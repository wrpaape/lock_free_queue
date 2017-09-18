set(
    SINGLE_WRTIER_RING_BUFFER_HEADERS
    ${PROJECT_INCLUDE_DIR}/ring_buffer/ring_buffer.hpp
)

if(BUILD_TESTING)
    add_subdirectory(test)
endif()
