/*
 * Copyright (c) 2013      Mellanox Technologies, Inc.
 *                         All rights reserved.
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 */
#include "oshmem_config.h"

#include "oshmem/constants.h"
#include "oshmem/include/shmem.h"

#include "oshmem/runtime/runtime.h"

#include "oshmem/mca/spml/spml.h"

/*
 * These routines provide a low latency mechanism to write basic types (short,
 * int, float, double, long) to symmetric data objects on remote PEs. 
 * The shmem_TYPE_p() routines write value to a symmetric array element or scalar
 * data object of the remote PE indicated by the parameter pe. These routines start the remote
 * transfer and may return before the data is delivered to the remote PE.
 */
#define SHMEM_TYPE_P(type_name, type)    \
    void shmem##type_name##_p(type *addr, type value, int pe) \
    {                                                               \
        int rc = OSHMEM_SUCCESS;                                    \
        size_t size = 0;                                            \
                                                                    \
        RUNTIME_CHECK_INIT();                                       \
        RUNTIME_CHECK_PE(pe);                                       \
        RUNTIME_CHECK_ADDR(addr);                                   \
                                                                    \
        size = sizeof(type);                                        \
        rc = MCA_SPML_CALL(put(                                     \
            (void*)addr,                                            \
            size,                                                   \
            (void*)&value,                                          \
            pe));                                                   \
        RUNTIME_CHECK_RC(rc);                                       \
                                                                    \
        return ;                                                    \
    }

#if OSHMEM_PROFILING
#pragma weak shmem_char_p = pshmem_char_p
#pragma weak shmem_short_p = pshmem_short_p
#pragma weak shmem_int_p = pshmem_int_p
#pragma weak shmem_long_p = pshmem_long_p
#pragma weak shmem_longlong_p = pshmem_longlong_p
#pragma weak shmem_float_p = pshmem_float_p
#pragma weak shmem_double_p = pshmem_double_p
#pragma weak shmem_longdouble_p = pshmem_longdouble_p
#include "oshmem/shmem/c/profile/defines.h"
#endif

SHMEM_TYPE_P(_char, char)
SHMEM_TYPE_P(_short, short)
SHMEM_TYPE_P(_int, int)
SHMEM_TYPE_P(_long, long)
SHMEM_TYPE_P(_longlong, long long)
SHMEM_TYPE_P(_float, float)
SHMEM_TYPE_P(_double, double)
SHMEM_TYPE_P(_longdouble, long double)
