if(PROJECT_CMAKE_HAVE_CPP_THREAD_LOCAL_INCLUDED)
    return()
endif()
set(PROJECT_CMAKE_HAVE_CPP_THREAD_LOCAL_INCLUDED TRUE)
# ==============================================================================
# check C++ compiler's support for C++11 'thread_local' storage class
# set HAVE_CPP_THREAD_LOCAL to nonzero if supported
# ==============================================================================
# External API
# ------------------------------------------------------------------------------
function(set_have_cpp_thread_local)
    set(build_dir ${PROJECT_CMAKE_BUILD_DIR}/have_cpp_thread_local)
    set(src_dir   ${PROJECT_CMAKE_SRC_DIR}/have_cpp_thread_local)

    try_compile(
        thread_local_compiled
        ${build_dir}
        ${src_dir}/test_have_cpp_thread_local.cpp
    )

    if(thread_local_compiled)
        set(HAVE_CPP_THREAD_LOCAL 1 PARENT_SCOPE)
    else()
        set(HAVE_CPP_THREAD_LOCAL 0 PARENT_SCOPE)
    endif()
endfunction()

set_have_cpp_thread_local()
