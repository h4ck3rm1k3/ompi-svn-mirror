/** @file 
 *
 *  
 */

/*
 * $HEADER$
 */

#ifndef MCA_PML_TEG_H
#define MCA_PML_TEG_H

#include "threads/thread.h"
#include "threads/condition.h"
#include "mem/free_list.h"
#include "util/cmd_line.h"
#include "request/request.h"
#include "mca/pml/pml.h"
#include "mca/pml/base/pml_base_request.h"
#include "mca/ptl/ptl.h"


/**
 * TEG PML Interface
 */

struct mca_pml_teg_t {
    mca_pml_t super; 

    mca_ptl_base_module_t **teg_ptl_modules;
    size_t teg_num_ptl_modules;

    mca_ptl_t** teg_ptls;
    size_t teg_num_ptls;

    lam_list_t   teg_procs;
    lam_mutex_t  teg_lock;

    int teg_free_list_num;   /* initial size of free list */
    int teg_free_list_max;   /* maximum size of free list */
    int teg_free_list_inc;   /* number of elements to grow free list */
    int teg_poll_iterations; /* number of iterations to poll for completion */

    /* free list of recv requests */
    lam_free_list_t teg_recv_requests;

    /* next recv sequence */
    mca_ptl_base_sequence_t teg_recv_sequence;

    /* request completion */
    lam_mutex_t teg_request_lock;
    lam_condition_t teg_request_cond;
    volatile int teg_request_waiting;

    /* null request */
    mca_pml_base_request_t teg_null;
};
typedef struct mca_pml_teg_t mca_pml_teg_t; 

extern mca_pml_teg_t mca_pml_teg;


/*
 * PML module functions.
 */

extern mca_pml_base_module_1_0_0_t mca_pml_teg_module;


extern int mca_pml_teg_module_open(void);
extern int mca_pml_teg_module_close(void);

extern mca_pml_t* mca_pml_teg_module_init(
    int *priority, 
    bool *allow_multi_user_threads,
    bool *have_hidden_threads
);

extern int mca_pml_teg_module_fini(void);



/*
 * PML interface functions.
 */

extern int mca_pml_teg_add_comm(
    struct lam_communicator_t* comm
);

extern int mca_pml_teg_del_comm(
    struct lam_communicator_t* comm
);

extern int mca_pml_teg_add_procs(
    struct lam_proc_t **procs,
    size_t nprocs
);

extern int mca_pml_teg_del_procs(
    struct lam_proc_t **procs,
    size_t nprocs
);

extern int mca_pml_teg_add_ptls(
    lam_list_t *ptls
);

extern int mca_pml_teg_isend_init(
    void *buf,
    size_t size,
    struct lam_datatype_t *datatype,
    int dst,
    int tag,
    mca_pml_base_send_mode_t mode,
    struct lam_communicator_t* comm,
    struct lam_request_t **request
);

extern int mca_pml_teg_isend(
    void *buf,
    size_t size,
    struct lam_datatype_t *datatype,
    int dst,
    int tag,
    mca_pml_base_send_mode_t mode,
    struct lam_communicator_t* comm,
    struct lam_request_t **request
);

extern int mca_pml_teg_send(
    void *buf,
    size_t size,
    struct lam_datatype_t *datatype,
    int dst,
    int tag,
    mca_pml_base_send_mode_t mode,
    struct lam_communicator_t* comm
);

extern int mca_pml_teg_irecv_init(
    void *buf,
    size_t size,
    struct lam_datatype_t *datatype,
    int src,
    int tag,
    struct lam_communicator_t* comm,
    struct lam_request_t **request
);

extern int mca_pml_teg_irecv(
    void *buf,
    size_t size,
    struct lam_datatype_t *datatype,
    int src,
    int tag,
    struct lam_communicator_t* comm,
    struct lam_request_t **request
);

extern int mca_pml_teg_recv(
    void *buf,
    size_t size,
    struct lam_datatype_t *datatype,
    int src,
    int tag,
    struct lam_communicator_t* comm,
    lam_status_public_t* status
);

extern int mca_pml_teg_progress(void);

extern int mca_pml_teg_start(
    lam_request_t** request
);

extern int mca_pml_teg_test(
    lam_request_t** request,
    int *completed,
    lam_status_public_t* status
);

extern int mca_pml_teg_wait(
    int count,
    lam_request_t** request,
    int *index,
    lam_status_public_t* status
);

extern int mca_pml_teg_waitall(
    int count,
    lam_request_t** request,
    lam_status_public_t* status
);

extern int mca_pml_teg_null(
    lam_request_t** request
);

extern int mca_pml_teg_free(
    lam_request_t** request
);

#endif

