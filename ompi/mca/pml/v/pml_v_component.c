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

#include "opal/mca/base/mca_base_param.h"
#include "opal/mca/base/base.h"
#include "opal/mca/base/mca_base_component_repository.h"
#include "ompi/constants.h"
#include "ompi/mca/pml/base/base.h"
#include "pml_v.h"
#include "vprotocol.h"
#include "base/base.h"

static int mca_pml_v_component_open(void);
static int mca_pml_v_component_close(void);
static int mca_pml_v_component_parasite_close(void);

static mca_pml_base_module_t *mca_pml_v_component_init(int* priority, bool enable_threads, bool enable_progress_threads);
static int mca_pml_v_component_finalize(void);
static int mca_pml_v_component_parasite_finalize(void);

static int mca_pml_v_enable(bool enable);

static inline int mca_pml_v_param_register_int( const char* param_name, int default_value);
static inline char *mca_pml_v_param_register_string( const char* param_name, char *default_value);

mca_pml_base_component_1_0_0_t mca_pml_v_component = 
{
  /* First, the mca_base_component_t struct containing meta
   * information about the component itself */
  {
    MCA_PML_BASE_VERSION_1_0_0, /* Indicate that we are a pml v1.0.0 component (which also implies a specific MCA version) */
    "v", /* MCA component name */
    1,   /* MCA component major version */
    0,   /* MCA component minor version */
    0,   /* MCA component release version */
    mca_pml_v_component_open,
    mca_pml_v_component_close
  },

  /* Next the MCA v1.0.0 component meta data */
  {
    false /* Whether the component is checkpointable or not */
  },

  mca_pml_v_component_init,  /* component init */
  mca_pml_v_component_finalize   /* component finalize */
};

static bool pml_v_enable_progress_treads = OMPI_ENABLE_PROGRESS_THREADS;
static bool pml_v_enable_mpi_threads = OMPI_ENABLE_MPI_THREADS;



/*******************************************************************************
 * MCA level functions - parasite setup
 */
static int mca_pml_v_component_open(void)
{
    char *output;
    int verbose;
    int priority;
    
    priority = mca_pml_v_param_register_int("priority", -1);
    output = mca_pml_v_param_register_string("output", "stderr");
    verbose = mca_pml_v_param_register_int("verbose", 0);

    pml_v_output_open(output, verbose);

    if(-1 != priority)
        V_OUTPUT_ERR("pml_v: Overriding priority setting (%d) with -1. The PML V should NEVER be the selected component; even when enabling fault tolerance.", priority);
            
    V_OUTPUT_VERBOSE(500, "loaded");
    return mca_vprotocol_base_open();
}
 
static int mca_pml_v_component_close(void)
{
    int ret;
   	
    V_OUTPUT_VERBOSE(500, "component_close: I don't want to be unloaded now.");

    /* Save original PML before making any changes  */
    mca_pml_v.host_pml_component = mca_pml_base_selected_component;
    mca_pml_v.host_pml = mca_pml;

    ret = mca_base_component_repository_retain_component("pml", "v");
    if(OPAL_SUCCESS != ret)
    {
        V_OUTPUT_ERR("pml_v: component_close: can't retain myself !");
        return ret;
    }
    
    /* Mark that we have changed something */ 
    snprintf(mca_pml_base_selected_component.pmlm_version.mca_component_name, 
             MCA_BASE_MAX_TYPE_NAME_LEN, "%s]v%s", 
             mca_pml_v.host_pml_component.pmlm_version.mca_component_name,
             mca_vprotocol_component.pmlm_version.mca_component_name);

    /* Replace finalize */
    mca_pml_base_selected_component.pmlm_finalize = 
        mca_pml_v_component_parasite_finalize;    
    
    /* Make sure we get initialized if some Vprotocol is enabled */
    mca_pml.pml_enable = mca_pml_v_enable;
    
    return OMPI_SUCCESS;
}

/*******************************************************************************
 * Parasite cleanup
 */
static int mca_pml_v_component_parasite_finalize(void)
{
    mca_base_component_list_item_t *cli = NULL;
    
    V_OUTPUT_VERBOSE(500, "parasite_finalize");
    
    /* Make sure we'll get closed again with the true close function */
    mca_pml_v_component.pmlm_version.mca_close_component = 
        mca_pml_v_component_parasite_close;
    cli = OBJ_NEW(mca_base_component_list_item_t);
    cli->cli_component = (mca_base_component_t *) &mca_pml_v_component;
    opal_list_prepend(&mca_pml_base_components_available, 
                      (opal_list_item_t *) cli);
    
    /* finalize vprotocol component */
    if(mca_vprotocol_base_selected())
        mca_vprotocol_component.pmlm_finalize();
    
    if(mca_pml_v.host_pml_component.pmlm_finalize != NULL)
        return mca_pml_v.host_pml_component.pmlm_finalize();
    else 
        return OMPI_SUCCESS;
}

static int mca_pml_v_component_parasite_close(void)
{
    V_OUTPUT_VERBOSE(500, "parasite_close: Ok, host component %s is closing, so I accept to die", 
                          mca_pml_v.host_pml_component.pmlm_version.mca_component_name);
    mca_pml_base_selected_component = mca_pml_v.host_pml_component;

    mca_vprotocol_base_close();    
    pml_v_output_close();

    mca_pml.pml_enable = mca_pml_v.host_pml.pml_enable;
    /* don't need to call the host component's close: pml_base will do it */
    return OMPI_SUCCESS; /* ignore any errors as we are leaving anyway */
}


/*******************************************************************************
 * Init/finalize for MCA PML components
 */
static mca_pml_base_module_t *mca_pml_v_component_init(int *priority,
                                                      bool enable_progress_threads,
                                                      bool enable_mpi_threads)
{
    V_OUTPUT_VERBOSE(1, "init: I'm not supposed to be here until BTL loading stuff gets fixed!? That's strange...");

    pml_v_enable_progress_treads = enable_progress_threads;
    pml_v_enable_mpi_threads = enable_mpi_threads;
    
    /* I NEVER want to be the selected PML, so I report less than possible 
     * priority and a NULL module 
     */
    *priority = -1;
    return NULL;
}

static int mca_pml_v_component_finalize(void)
{
    V_OUTPUT_VERBOSE(1, "finalize: I'm not supposed to be here until BTL loading stuff gets fixed!? That's strange...");
    /* Nothing to do here. We are not sure we need to be unloaded or not at 
     * this stage
     */
    return OMPI_SUCCESS;
}


/*******************************************************************************
 * Enable the PML V (and initialize the Vprotocol)
 */
static int mca_pml_v_enable(bool enable) 
{
    int ret;
    
    /* Enable the real PML (no threading issues there as threads are started 
     * later
     */ 
    ret = mca_pml_v.host_pml.pml_enable(enable);
    if(OMPI_SUCCESS != ret) return ret;
    
    if(enable) {
        if(mca_vprotocol_base_selected()) {
            if(mca_vprotocol.enable)
                return mca_vprotocol.enable(enable);
            else 
                return OMPI_SUCCESS;
        }

        mca_vprotocol_base_select(pml_v_enable_progress_treads, 
                                  pml_v_enable_mpi_threads);
        
        if(mca_vprotocol_base_selected()) {
            V_OUTPUT_VERBOSE(1, "I don't want to die: I will parasite %s host component %s with %s %s", 
                             mca_pml_base_selected_component.pmlm_version.mca_type_name,
                             mca_pml_base_selected_component.pmlm_version.mca_component_name,
                             mca_vprotocol_component.pmlm_version.mca_type_name,
                             mca_vprotocol_component.pmlm_version.mca_component_name);
        
            ret = mca_vprotocol_base_parasite();
            if(OMPI_SUCCESS != ret) return ret;
            if(mca_vprotocol.enable)
                return mca_vprotocol.enable(enable);
            else
                return OMPI_SUCCESS;
        }
        V_OUTPUT_VERBOSE(1, "No fault tolerant protocol selected. All are unloaded");
    }
    /* Disable */
    mca_pml = mca_pml_v.host_pml;
    mca_pml.pml_enable = mca_pml_v_enable;
    return OMPI_SUCCESS;
}


/*******************************************************************************
 * utilities
 */
static inline int mca_pml_v_param_register_int( const char* param_name,
                                                  int default_value )
{
    int id = mca_base_param_register_int("pml", "v", param_name, NULL, default_value);
    int param_value = default_value;
    mca_base_param_lookup_int(id, &param_value);
    return param_value;
}

static inline char *mca_pml_v_param_register_string( const char* param_name,
                                                  char *default_value )
{
    int id = mca_base_param_register_string("pml", "v", param_name, NULL, default_value);
    char *param_value = default_value;
    mca_base_param_lookup_string(id, &param_value);
    return param_value;
}
