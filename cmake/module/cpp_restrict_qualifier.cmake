if(PROJECT_CMAKE_CPP_RESTRICT_QUALIFIER_INCLUDED)
    return()
endif()
set(PROJECT_CMAKE_CPP_RESTRICT_QUALIFIER_INCLUDED TRUE)
# ==============================================================================
# check C++ compiler's support for a 'restrict' extension keyword
# set CPP_RESTRICT_QUALIFIER to that keyword or empty string if not supported
# ==============================================================================
# External API
# ------------------------------------------------------------------------------
function(set_cpp_restrict_qualifier)
    set(build_dir ${PROJECT_CMAKE_BUILD_DIR}/cpp_restrict_qualifier)
    set(src_dir   ${PROJECT_CMAKE_SRC_DIR}/cpp_restrict_qualifier)
    foreach(qualifer __restrict__ __restrict restrict)
        try_compile(
            qualifier_compiled
            ${build_dir}
            ${src_dir}/test_cpp_restrict_qualifier.cpp
            COMPILE_DEFINITIONS "-DCPP_RESTRICT_QUALIFIER=${qualifer}"
        )

        if(qualifier_compiled)
            set(supported_qualifier ${qualifer})
            break()
        endif()
    endforeach()

    set(CPP_RESTRICT_QUALIFIER "${supported_qualifier}" PARENT_SCOPE)
endfunction()

set_cpp_restrict_qualifier()
