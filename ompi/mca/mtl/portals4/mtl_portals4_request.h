/*
 * Copyright (c) 2004-2006 The Trustees of Indiana University and Indiana
 *                         University Research and Technology
 *                         Corporation.  All rights reserved.
 * Copyright (c) 2004-2005 The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 * Copyright (c) 2004-2005 High Performance Computing Center Stuttgart,
 *                         University of Stuttgart.  All rights reserved.
 * Copyright (c) 2004-2005 The Regents of the University of California.
 *                         All rights reserved.
 * Copyright (c) 2010      Sandia National Laboratories.  All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

#ifndef OMPI_MTL_PORTALS_REQUEST_H
#define OMPI_MTL_PORTALS_REQUEST_H

#include "opal/datatype/opal_convertor.h"
#include "ompi/mca/mtl/mtl.h"


struct ompi_mtl_portals4_base_request_t {
    struct mca_mtl_request_t super;
    int (*event_callback)(ptl_event_t *ev, struct ompi_mtl_portals4_base_request_t*);
};
typedef struct ompi_mtl_portals4_base_request_t ompi_mtl_portals4_base_request_t;


struct ompi_mtl_portals4_send_request_t {
    ompi_mtl_portals4_base_request_t super;
    void *buffer_ptr;
    ptl_handle_md_t md_h;
    ptl_handle_me_t me_h;
    volatile int event_count;
};
typedef struct ompi_mtl_portals4_send_request_t ompi_mtl_portals4_send_request_t;


struct ompi_mtl_portals4_recv_request_t {
    ompi_mtl_portals4_base_request_t super;
    void *buffer_ptr;
    ptl_handle_md_t md_h;
    ptl_handle_me_t me_h;
    ptl_handle_ct_t ct_h;
    struct opal_convertor_t *convertor;
    void *delivery_ptr;
    size_t delivery_len;
};
typedef struct ompi_mtl_portals4_recv_request_t ompi_mtl_portals4_recv_request_t;


struct ompi_mtl_portals4_probe_request_t {
    ompi_mtl_portals4_base_request_t super;
    volatile int req_complete;
    int found_match;
    struct ompi_status_public_t status;
};
typedef struct ompi_mtl_portals4_probe_request_t ompi_mtl_portals4_probe_request_t;


struct ompi_mtl_portals4_request_t {
    union {
        ompi_mtl_portals4_send_request_t send;
        ompi_mtl_portals4_recv_request_t recv;
        ompi_mtl_portals4_probe_request_t probe;
    } u;
};
typedef struct ompi_mtl_portals4_request_t ompi_mtl_portals4_request_t;


#endif