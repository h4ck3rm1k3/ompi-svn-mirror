/*
 * Copyright (c) 2004-2005 The Trustees of Indiana University.
 *                         All rights reserved.
 * Copyright (c) 2004-2005 The Trustees of the University of Tennessee.
 *                         All rights reserved.
 * Copyright (c) 2004-2005 High Performance Computing Center Stuttgart, 
 *                         University of Stuttgart.  All rights reserved.
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 */
/**
 * @file
 *
 * I/O Forwarding Service
 */
                                                                                         
#ifndef MCA_IOF_BASE_H
#define MCA_IOF_BASE_H

#include "ompi_config.h"
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_UIO_H
#include <sys/uio.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "include/ompi.h"
#include "class/ompi_free_list.h"
#include "threads/condition.h"
#include "mca/mca.h"
#include "mca/iof/iof.h"

#if defined(c_plusplus) || defined(__cplusplus)
extern "C" {
#endif


struct orte_iof_base_t {
   int                  iof_output;
   ompi_list_t          iof_components_opened;
   ompi_list_t          iof_endpoints;
   ompi_mutex_t         iof_lock;
   ompi_condition_t     iof_condition;
   size_t               iof_waiting;
   ompi_free_list_t     iof_fragments;
   size_t               iof_window_size;
   orte_process_name_t* iof_service;
};
typedef struct orte_iof_base_t orte_iof_base_t;



int orte_iof_base_open(void);
int orte_iof_base_close(void);
int orte_iof_base_select(void);
int orte_iof_base_flush(void);


extern orte_iof_base_t orte_iof_base;

#if defined(c_plusplus) || defined(__cplusplus)
}
#endif
#endif /* MCA_PML_H */
