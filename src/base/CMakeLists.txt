
set(SCL_BASE_SOURCES
    scl_memmgr.cc
    scl_trace_fprintf.c
    sc_getopt.cc
    sc_benchmark.cc
)

if(MSVC OR BORLAND)
add_definitions( -DSCL_BASE_DLL_EXPORTS )
endif()

SCL_ADDLIB(base "${SCL_BASE_SOURCES}" "")
