
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <math.h>
#include <sys/time.h>
#include "mpi.h"
#include "test_util.h"

#define MYBUFSIZE (4*1024*1024)

char        s_buf[MYBUFSIZE];
char        r_buf[MYBUFSIZE];
int         skip = 40;

int
main (int argc, char *argv[])
{
    int         myid, numprocs, i, j;
    double      startwtime = 0.0, endwtime;
    int         namelen;
    int         size;
    MPI_Status  stat;
    int         loop = 1;
    int         sleep = 1;

    struct timeval t_start, t_end;

    if (argc < 2) {
        fprintf (stderr, "Usage: %s msg_size\n", argv[0]);
        return 0;
    } else {
	size = atoi (argv[1]);
	if (argc > 2) 
	    loop = atoi (argv[2]);
    }

    /* Get some environmental variables set for Open MPI, OOB */
    env_init_for_elan();

    MPI_Init (&argc, &argv);
    MPI_Comm_size (MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank (MPI_COMM_WORLD, &myid);

    for (i = 0; i < loop + skip; i++) {
	if (i == skip)
	    gettimeofday (&t_start, 0);
	if (myid == 0) {
            MPI_Send (s_buf, size, MPI_CHAR, 1, i, MPI_COMM_WORLD);
            MPI_Recv (r_buf, size, MPI_CHAR, 1, i, MPI_COMM_WORLD, &stat);
	} else {
            MPI_Recv (r_buf, size, MPI_CHAR, 0, i, MPI_COMM_WORLD, &stat);
            MPI_Send (s_buf, size, MPI_CHAR, 0, i, MPI_COMM_WORLD);
        }
    }
    gettimeofday (&t_end, 0);

    if (myid == 0) {
        double      latency;
        latency = ((1.0e6 * t_end.tv_sec + t_end.tv_usec) 
		- (1.0e6 * t_start.tv_sec + t_start.tv_usec)) / (2.0 * loop);
	fprintf(stdout, "length %d latency %8f\n", 
		size, latency);
	fflush(stdout);
    }

    MPI_Finalize ();
    return 0;
}
