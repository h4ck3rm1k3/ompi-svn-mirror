/*
 * $HEADER$
 */

#include "lam_config.h"
#include <stdio.h>

#include "mpi.h"
#include "mpi/interface/c/bindings.h"

#if LAM_HAVE_WEAK_SYMBOLS && LAM_PROFILING_DEFINES
#pragma weak MPI_Allreduce = PMPI_Allreduce
#endif

int MPI_Allreduce(void *sendbuf, void *recvbuf, int count,
		           MPI_Datatype datatype, MPI_Op op, MPI_Comm comm) {
	return MPI_SUCCESS;
}

