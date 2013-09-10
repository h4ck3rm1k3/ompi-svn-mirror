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
#include "oshmem/shmem/f77/bindings.h"
#include "oshmem/include/shmem.h"
#include "oshmem/shmem/shmem_api_logger.h"
#include "oshmem/runtime/runtime.h"
#include "oshmem/mca/spml/spml.h"
#include "stdio.h"

OMPI_GENERATE_F77_BINDINGS (void,
        SHMEM_GET64,
        shmem_get64_,
        shmem_get64__,
        shmem_get64_f,
        (FORTRAN_POINTER_T target, FORTRAN_POINTER_T source, MPI_Fint *len, MPI_Fint *pe), 
        (target,source,len,pe) )

void shmem_get64_f(FORTRAN_POINTER_T target, FORTRAN_POINTER_T source, MPI_Fint *len, MPI_Fint *pe)
{
    MCA_SPML_CALL(get(FPTR_2_VOID_PTR(source), 
        OMPI_FINT_2_INT(*len) * 8, 
        FPTR_2_VOID_PTR(target), 
        OMPI_FINT_2_INT(*pe)));
}
  
