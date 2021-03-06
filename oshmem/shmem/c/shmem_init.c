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

#include <stdlib.h>

#include "orte/util/show_help.h"

#include "opal/runtime/opal_cr.h"
#include "opal/util/output.h"

#include "oshmem/constants.h" 
#include "oshmem/include/shmem.h"
#include "oshmem/runtime/params.h"
#include "oshmem/runtime/runtime.h"
#include "oshmem/shmem/shmem_api_logger.h"

#if OSHMEM_PROFILING
#pragma weak start_pes = pstart_pes
#include "oshmem/shmem/c/profile/defines.h"
#endif

void start_pes(int npes)
{
    /* spec says that npes are ignored for now */
    shmem_init();
}

void shmem_init(void)
{
    int err = OSHMEM_SUCCESS;
    int provided;
    int required = SHMEM_THREAD_SINGLE;

    if (oshmem_shmem_initialized) {
        /*
         * SPEC: If start_pes() is called multiple times, subsequent calls have no effect.
         */
        return;
    }

    err = oshmem_shmem_init(0, NULL, required, &provided);
    if (OSHMEM_SUCCESS != err) {
        /* since spec does not propagete error to user we can only abort */
        SHMEM_API_ERROR("SHMEM failed to initialize - aborting");
        oshmem_shmem_abort(-1);
    }

    OPAL_CR_INIT_LIBRARY();
}

