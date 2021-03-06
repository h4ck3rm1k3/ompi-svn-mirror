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

#include "oshmem/op/op.h"
#include "oshmem/mca/atomic/atomic.h"

/*
 * These routines perform an atomic increment operation on a remote data object. 
 * The atomic increment routines replace the value of target with its value incremented by
 * one. The operation must be completed without the possibility of another process updating
 * target between the time of the fetch and the update.
 */
#define SHMEM_TYPE_INC(type_name, type)    \
    void shmem##type_name##_inc(type *target, int pe) \
    {                                                               \
        int rc = OSHMEM_SUCCESS;                                    \
        size_t size = 0;                                            \
        type value = 1;                                             \
        type out_value;                                             \
        oshmem_op_t* op = oshmem_op_sum##type_name;                 \
                                                                    \
        RUNTIME_CHECK_INIT();                                       \
        RUNTIME_CHECK_PE(pe);                                       \
        RUNTIME_CHECK_ADDR(target);                                 \
                                                                    \
        size = sizeof(out_value);                                   \
        rc = MCA_ATOMIC_CALL(fadd(                                  \
            (void*)target,                                          \
            NULL,                                                   \
            (const void*)&value,                                    \
            size,                                                   \
            pe,                                                     \
            op));                                                   \
        RUNTIME_CHECK_RC(rc);                                       \
                                                                    \
        return ;                                                    \
    }

#if OSHMEM_PROFILING
#pragma weak shmem_int_inc = pshmem_int_inc
#pragma weak shmem_long_inc = pshmem_long_inc
#pragma weak shmem_longlong_inc = pshmem_longlong_inc
#include "oshmem/shmem/c/profile/defines.h"
#endif

SHMEM_TYPE_INC(_int, int)
SHMEM_TYPE_INC(_long, long)
SHMEM_TYPE_INC(_longlong, long long)
