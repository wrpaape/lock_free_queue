if(PROJECT_CMAKE_ADD_CUSTOM_TEST_CMAKE_INCLUDED)
    return()
endif()
set(PROJECT_CMAKE_ADD_CUSTOM_TEST_CMAKE_INCLUDED TRUE)
# ==============================================================================
# add a ${NAME} executable and CTest
# ==============================================================================
# External Dependencies
# ------------------------------------------------------------------------------
include(parse_arguments)


# External API
# ------------------------------------------------------------------------------
function(add_custom_test)
    set( 
        boolean_keywords  ""
    )
    set(
        value_keywords    NAME
                          FRAMEWORK_NAME
    )
    set(
        list_keywords     SOURCES
                          LIBRARIES
                          DEPENDENCIES
                          INCLUDE_DIRECTORIES
                          FRAMEWORK_SOURCES
                          FRAMEWORK_LIBRARIES
                          FRAMEWORK_DEPENDENCIES
                          FRAMEWORK_INCLUDE_DIRECTORIES
    )
    set(
        required_keywords NAME
                          SOURCES
    )
    parse_arguments(
        CUSTOM_TEST            # prefix
        "${boolean_keywords}"
        "${value_keywords}"
        "${list_keywords}"
        "${required_keywords}"
        ${ARGV}
    )

    # check for required end user parameters
    if(CUSTOM_TEST_MISSING_REQUIRED_ARGUMENTS)
        if (CUSTOM_TEST_FRAMEWORK_NAME)
            set(CUSTOM_TEST_TYPE "\"${CUSTOM_TEST_FRAMEWORK_NAME}\"")
        else()
            set(CUSTOM_TEST_TYPE "custom")
        endif()
        message(
            FATAL_ERROR
            "add_custom_test -- failed to add a ${CUSTOM_TEST_TYPE} test"
        )
    endif()

    add_executable(
        ${CUSTOM_TEST_NAME}
        ${CUSTOM_TEST_SOURCES}
        ${CUSTOM_TEST_FRAMEWORK_SOURCES}
    )

    target_include_directories(
        ${CUSTOM_TEST_NAME}
        PUBLIC
        ${CUSTOM_TEST_INCLUDE_DIRECTORIES}
        ${CUSTOM_TEST_FRAMEWORK_INCLUDE_DIRECTORIES}
        ${PROJECT_TEST_INCLUDE_DIR}
        ${PROJECT_INCLUDE_DIR}
    )

    list(APPEND CUSTOM_TEST_LIBRARIES ${CUSTOM_TEST_FRAMEWORK_LIBRARIES})
    if (CUSTOM_TEST_LIBRARIES)
        target_link_libraries(${CUSTOM_TEST_NAME} ${CUSTOM_TEST_LIBRARIES})
    endif()


    list(APPEND CUSTOM_TEST_DEPENDENCIES ${CUSTOM_TEST_FRAMEWORK_NAME}
                                         ${CUSTOM_TEST_FRAMEWORK_DEPENDENCIES})
    if (CUSTOM_TEST_DEPENDENCIES)
        add_dependencies(${CUSTOM_TEST_NAME} ${CUSTOM_TEST_DEPENDENCIES})
    endif()

    set_target_properties(
        ${CUSTOM_TEST_NAME}
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${PROJECT_TEST_BIN_DIR}
        COMPILE_DEFINITIONS      BUILD_TESTING
    )

    add_test(
        NAME              ${CUSTOM_TEST_NAME}
        COMMAND           ${CUSTOM_TEST_NAME}
        WORKING_DIRECTORY ${PROJECT_TEST_BIN_DIR}
    )

    # append to dependencies of 'make build_test', etc..
    foreach(target ${PROJECT_BUILD_TEST_COMMANDS})
        add_dependencies(${target} ${CUSTOM_TEST_NAME})
    endforeach()
endfunction()
