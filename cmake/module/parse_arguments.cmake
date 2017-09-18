if(PROJECT_CMAKE_PARSE_ARGUMENTS_CMAKE_INCLUDED)
    return()
endif()
set(PROJECT_CMAKE_PARSE_ARGUMENTS_CMAKE_INCLUDED TRUE)
# ==============================================================================
# parse function/macro named arguments, log warn for unparsed arguments and
# error for missing required arguments
# ==============================================================================
# External Dependencies
# ------------------------------------------------------------------------------
include(CMakeParseArguments)


# External API
# ------------------------------------------------------------------------------
macro(parse_arguments _prefix
                      _option_keywords
                      _one_value_keywords
                      _multi_value_keywords
                      _required_keywords) # /* arguments... */

    cmake_parse_arguments(
        "${_prefix}"
        "${_option_keywords}"      # booleans
        "${_one_value_keywords}"   # single values
        "${_multi_value_keywords}" # lists
        ${ARGN}
    )

    # check for required end user parameters
    foreach(_required ${_required_keywords})
        if(NOT DEFINED ${_prefix}_${_required})
            list(APPEND ${_prefix}_MISSING_REQUIRED_ARGUMENTS ${_required})
        endif()
    endforeach()

    if(${_prefix}_MISSING_REQUIRED_ARGUMENTS)
        message(
            SEND_ERROR
            "parse_arguments -- missing required arguments for keywords: "
            ${${_prefix}_MISSING_REQUIRED_ARGUMENTS}
        )
    endif()

    if(${${_prefix}_UNPARSED_ARGUMENTS})
        message(
            WARNING
            "parse_arguments -- ignoring unparsed arguments: "
            ${${_prefix}_UNPARSED_ARGUMENTS}
        )
    endif()
endmacro()
