/*
 * Copyright (c) 2004-2005 The Trustees of Indiana University and Indiana
 *                         University Research and Technology
 *                         Corporation.  All rights reserved.
 * Copyright (c) 2004-2006 The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 * Copyright (c) 2004-2005 High Performance Computing Center Stuttgart, 
 *                         University of Stuttgart.  All rights reserved.
 * Copyright (c) 2004-2005 The Regents of the University of California.
 *                         All rights reserved.
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 */

#ifndef OPAL_SYS_LIMITS_H
#define OPAL_SYS_LIMITS_H

#include "opal_config.h"

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#if defined(c_plusplus) || defined(__cplusplus)
extern "C" {
#endif

/* define a structure to hold the various limits we find
 * so that users can neatly access them
 */
typedef struct opal_sys_limits_t {
    bool initialized;
    int num_files;
    int num_procs;
    size_t file_size;
} opal_sys_limits_t;

/* since we only want to do this once, we will store the
 * values in the following locations - provide access here
 */
OPAL_DECLSPEC extern opal_sys_limits_t opal_sys_limits;
    
/* Get the system resource limits and, if requested, set
 * them to the max allowed
 */
OPAL_DECLSPEC int opal_util_init_sys_limits(void);

#if defined(c_plusplus) || defined(__cplusplus)
}
#endif

#endif /* OPAL_STRNCPY_H */
