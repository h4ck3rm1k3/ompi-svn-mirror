/*
 * Copyright (c) 2004-2007 The Trustees of the University of Tennessee.
 *                         All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

#include "ompi_config.h"

#include "orte/util/output.h"

#include "pml_v_output.h"

#if defined(HAVE_UNISTD_H)
#include <unistd.h>
#endif
#if defined(HAVE_STRING_H)
#include <string.h>
#endif

int pml_v_output = -1;

int pml_v_output_open(char *output, int verbosity) {
    opal_output_stream_t lds;
    char hostname[32] = "NA";
  
    OBJ_CONSTRUCT(&lds, opal_output_stream_t);
    if(!output) {
      pml_v_output = 0; 
    } 
    else {
        if(!strcmp(output, "stdout")) {
            lds.lds_want_stdout = true;
        } 
        else if(!strcmp(output, "stderr")) {
            lds.lds_want_stderr = true;
        }
        else 
        {
            lds.lds_want_file = true;
            lds.lds_file_suffix = output;
        }
        lds.lds_is_debugging = true;
        gethostname(hostname, 32);
        asprintf(&lds.lds_prefix, "[%s:%05d] pml_v: ", hostname, getpid());
        lds.lds_verbose_level = verbosity;
        pml_v_output = orte_output_open(&lds, "PML", "V", "DEBUG", NULL);
        free(lds.lds_prefix);
    }
    return pml_v_output;
}

void pml_v_output_close(void) {
    orte_output_close(pml_v_output);
}
