/* -*- C -*-
 *
 * Copyright (c) 2004-2005 The Trustees of Indiana University.
 *                         All rights reserved.
 * Copyright (c) 2004-2005 The Trustees of the University of Tennessee.
 *                         All rights reserved.
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
/**
 * @file
 *
 * Data packing subsystem.
 */

#ifndef ORTE_DPS_H_
#define ORTE_DPS_H_

#include "orte_config.h"

#include "include/orte_constants.h"

#include "dps/dps_types.h"

#if defined(c_plusplus) || defined(__cplusplus)
extern "C" {
#endif
/**
 * Top-level itnerface function to pack one or more values into a
 * buffer.
 *
 * The pack function packs one or more values of a specified type into
 * the specified buffer.  The buffer must have already been
 * initialized via an OBJ_NEW or OBJ_CONSTRUCT call - otherwise, the
 * pack_value function will return an error. Providing an unsupported
 * type flag will likewise be reported as an error.
 * 
 * Note that any data to be packed that is not hard type cast (i.e.,
 * not type cast to a specific size) may lose precision when unpacked
 * by a non-homogeneous recipient.  The DPS will do its best to deal
 * with heterogeneity issues between the packer and unpacker in such
 * cases. Sending a number larger than can be handled by the recipient
 * will return an error code (generated by the DPS upon unpacking) via
 * the RML upon transmission - the DPS cannot detect such errors
 * during packing.
 * 
 * @param *buffer A pointer to the buffer into which the value is to
 * be packed.
 *
 * @param *src A void* pointer to the data that is to be packed. Note
 * that strings are to be passed as (char **) - i.e., the caller must
 * pass the address of the pointer to the string as the void*. This
 * allows the DPS to use a single interface function, but still allow
 * the caller to pass multiple strings in a single call.
 *
 * @param num A size_t value indicating the number of values that are
 * to be packed, beginning at the location pointed to by src. A string
 * value is counted as a single value regardless of length. The values
 * must be contiguous in memory. Arrays of pointers (e.g., string
 * arrays) should be contiguous, although (obviously) the data pointed
 * to need not be contiguous across array entries.
 *
 * @param type The type of the data to be packed - must be one of the
 * DPS defined data types.
 * 
 * @retval ORTE_SUCCESS The data was packed as requested.
 *
 * @retval ORTE_ERROR(s) An appropriate ORTE error code indicating the
 * problem encountered. This error code should be handled
 * appropriately.
 * 
 * @code
 * orte_buffer_t *buffer;
 * int32_t src;
 * 
 * status_code = orte_dps.pack(buffer, &src, 1, ORTE_INT32);
 * @endcode
 */
typedef int (*orte_dps_pack_fn_t)(orte_buffer_t *buffer, void *src,
                                  size_t num_values,
                                  orte_data_type_t type);

/**
 * Unpack values from a buffer.
 *
 * The unpack function unpacks the next value (or values) of a
 * specified type from the specified buffer.
 *
 * The buffer must have already been initialized via an OBJ_NEW or
 * OBJ_CONSTRUCT call (and assumedly filled with some data) -
 * otherwise, the unpack_value function will return an
 * error. Providing an unsupported type flag will likewise be reported
 * as an error, as will specifying a data type that DOES NOT match the
 * type of the next item in the buffer. An attempt to read beyond the
 * end of the stored data held in the buffer will also return an
 * error.
 * 
 * NOTE: it is possible for the buffer to be corrupted and that
 * the DPS will *think* there is a proper variable type at the
 * beginning of an unpack region - but that the value is bogus (e.g., just
 * a byte field in a string array that so happens to have a value that
 * matches the specified data type flag). Therefore, the data type error check
 * is NOT completely safe. This is true for ALL unpack functions.
 *
 * 
 * Unpacking values is a "destructive" process - i.e., the values are
 * removed from the buffer, thus reducing the buffer size. It is
 * therefore not possible for the caller to re-unpack a value from the
 * same buffer.
 * 
 * Warning: The caller is responsible for providing adequate memory
 * storage for the requested data. The orte_dps_peek() function is
 * provided to assist in meeting this requirement. As noted below, the user
 * must provide a parameter indicating the maximum number of values that
 * can be unpacked into the allocated memory. If more values exist in the
 * buffer than can fit into the memory storage, then the dps will unpack
 * what it can fit into that location and return an error code indicating
 * that the buffer was only partially unpacked.
 * 
 * Note that any data that was not hard type cast (i.e., not type cast
 * to a specific size) when packed may lose precision when unpacked by
 * a non-homogeneous recipient.  The DPS will do its best to deal with
 * heterogeneity issues between the packer and unpacker in such
 * cases. Sending a number larger than can be handled by the recipient
 * will return an error code (generated by the DPS upon unpacking) via
 * the RML upon transmission - the DPS cannot detect such errors
 * during packing.
 * 
 * @param *buffer A pointer to the buffer from which the value will be
 * extracted.
 *
 * @param *dest A void* pointer to the memory location into which the
 * data is to be stored. Note that these values will be stored
 * contiguously in memory. For strings, this pointer must be to (char
 * **) to provide a means of supporting multiple string
 * operations. The DPS unpack function will allocate memory for each
 * string in the array - the caller must only provide adequate memory
 * for the array of pointers.
 *
 * @param *num A pointer to a size_t value indicating the maximum
 * number of values that are to be unpacked, beginning at the location
 * pointed to by src. This is provided to help protect the caller from
 * memory overrun. Note that a string
 * value is counted as a single value regardless of length.
 * 
 * @note The unpack function will return the actual number of values
 * unpacked in this location.
 * 
 * @param type The type of the data to be unpacked - must be one of
 * the DPS defined data types.
 * 
 * @retval *max_num_values The number of values actually unpacked. In
 * most cases, this should match the maximum number provided in the
 * parameters - but in no case will it exceed the value of this
 * parameter.  Note that if you unpack fewer values than are actually
 * available, the buffer will be in an unpackable state - the dps will
 * return an error code to warn of this condition.
 * 
 * @retval ORTE_SUCCESS The next item in the buffer was successfully
 * unpacked.
 * 
 * @retval ORTE_ERROR(s) The unpack function returns an error code
 * under one of several conditions: (a) the number of values in the
 * item exceeds the max num provided by the caller; (b) the type of
 * the next item in the buffer does not match the type specified by
 * the caller; or (c) the unpack failed due to either an error in the
 * buffer or an attempt to read past the end of the buffer.
 * 
 * @code
 * orte_buffer_t *buffer;
 * int32_t dest;
 * char **string_array;
 * size_t num_values;
 * 
 * num_values = 1;
 * status_code = orte_dps.unpack(buffer, (void*)&dest, &num_values, ORTE_INT32);
 * 
 * num_values = 5;
 * string_array = malloc(num_values*sizeof(char *));
 * status_code = orte_dps.unpack(buffer, (void*)(string_array), &num_values, ORTE_STRING);
 * 
 * @endcode
 */
typedef int (*orte_dps_unpack_fn_t)(orte_buffer_t *buffer, void *dest,
                                    size_t *max_num_values,
                                    orte_data_type_t type);

/**
 * Get the type and number of values of the next item in the buffer.
 *
 * The peek function looks at the next item in the buffer and returns
 * both its type and the number of values in the item. This is a
 * non-destructive function call that does not disturb the buffer, so
 * it can be called multiple times if desired.
 * 
 * @param buffer A pointer to the buffer in question.
 *
 * @param type A pointer to an orte_data_type_t variable where the
 * type of the next item in the buffer is to be stored. Caller must
 * have memory backing this location.
 *
 * @param number A pointer to a size_t variable where the number of
 * data values in the next item is to be stored. Caller must have
 * memory backing this location.
 * 
 * @retval ORTE_SUCCESS Requested info was successfully returned.
 * @retval ORTE_ERROR(s) An appropriate error code indicating the
 * problem will be returned. This should be handled appropriately by
 * the caller.
 * 
 */
typedef int (*orte_dps_peek_next_item_fn_t)(orte_buffer_t *buffer,
                                            orte_data_type_t *type,
                                            size_t *number);

/**
 * Unload the data payload from a buffer.
 *
 * The unload function provides the caller with a pointer to the data
 * payload within the buffer and the size of that payload. This allows
 * the user to directly access the payload - typically used in the RML
 * to unload the payload from the buffer for transmission.
 * 
 * @note This is a destructive operation. While the payload is
 * undisturbed, the function will clear the buffer's pointers to the
 * payload. Thus, the buffer and the payload are completely separated,
 * leaving the caller free to OBJ_RELEASE the buffer.
 * 
 * @param buffer A pointer to the buffer whose payload is to be
 * unloaded.
 *
 * @param payload The address to a void* pointer that is to be loaded
 * with the address of the data payload in the buffer.
 *
 * @param size The size (in bytes) of the data payload in the buffer.
 * 
 * @retval ORTE_SUCCESS The request was succesfully completed.
 *
 * @retval ORTE_ERROR(s) An appropriate error code indicating the
 * problem will be returned. This should be handled appropriately by
 * the caller.
 *
 * @code
 * orte_buffer_t *buffer;
 * uint8_t *bytes;
 * size_t size;
 * 
 * status_code = orte_dps.unload(buffer, (void**)(&bytes), &size);
 * OBJ_RELEASE(buffer);
 * @endcode
 */
typedef int (*orte_dps_unload_fn_t)(orte_buffer_t *buffer,
                                    void **payload,
                                    size_t *size);

/**
 * Load a data payload into a buffer.
 *
 * The load function allows the caller to replace the payload in a
 * buffer with one provided by the caller. If a payload already exists
 * in the buffer, the function will "free" the existing data to
 * release it, and then replace the data payload with the one provided
 * by the caller.
 * 
 * @note The buffer must be allocated in advance via the OBJ_NEW
 * function call - failing to do so will cause the load function to
 * return an error code.
 * 
 * @note The caller is responsible for pre-packing the provided
 * payload - the load function cannot convert to network byte order
 * any data contained in the provided payload.
 * 
 * @param buffer A pointer to the buffer into which lthe payload is to
 * be loaded.
 *
 * @param payload A void* pointer to the payload to be loaded into the
 * buffer.
 *
 * @param size The size (in bytes) of the provided payload.
 * 
 * @retval ORTE_SUCCESS The request was successfully completed
 *
 * @retval ORTE_ERROR(s) An appropriate error code indicating the
 * problem will be returned. This should be handled appropriately by
 * the caller.
 *
 * @code
 * orte_buffer_t *buffer;
 * uint8_t bytes;
 * size_t size;
 * 
 * buffer = OBJ_NEW(orte_buffer_t);
 * status_code = orte_dps.load(buffer, (void*)(&bytes), size);
 * @endcode
 */
typedef int (*orte_dps_load_fn_t)(orte_buffer_t *buffer,
                                  void *payload,
                                  size_t size);


/**
 * DPS initialization function.
 *
 * In dynamic libraries, declared objects and functions don't get
 * loaded until called. We need to ensure that the orte_dps function
 * structure gets loaded, so we provide an "open" call that is
 * executed as part of the program startup.
 */
OMPI_DECLSPEC int orte_dps_open(void);

/**
 * Register a pack/unpack function pair.
 *
 * @param pack_fn [IN] Function pointer to the pack routine
 * @param unpack_fn [IN] Function pointer to the unpack routine
 * @param name [IN] String name for this pair (mainly for debugging)
 * @param type [OUT] Type number for this registration
 *
 * @returns ORTE_SUCCESS upon success
 *
 * This function registers a pack/unpack function pair for a specific
 * type. This
 * is most useful when extending the datatypes that the DPS can
 * handle; pack and unpack functions can nest calls to orte_dps.pack()
 * / orte_dps.unpack(), so defining small pack/unpack functions can be
 * used recursively to build larger types (e.g., packing/unpacking
 * structs can use calls to orte_dps.pack()/unpack() to serialize /
 * deserialize individual members).
 */
typedef int (*orte_dps_register_fn_t)(orte_dps_pack_fn_t pack_fn,
                                    orte_dps_unpack_fn_t unpack_fn,
                                    const char *name, orte_data_type_t type);

/**
 * DPS finalize function
 */
OMPI_DECLSPEC int orte_dps_close(void);


/**
 * Base structure for the DPS
 *
 * Base module structure for the DPS - presents the required function
 * pointers to the calling interface. 
 */
struct orte_dps_t {
    orte_dps_pack_fn_t pack;
    orte_dps_unpack_fn_t unpack;
    orte_dps_peek_next_item_fn_t peek;
    orte_dps_unload_fn_t unload;
    orte_dps_load_fn_t load;
    orte_dps_register_fn_t register_type;
};
typedef struct orte_dps_t orte_dps_t;

#if defined(c_plusplus) || defined(__cplusplus)
}
#endif

OMPI_DECLSPEC extern orte_dps_t orte_dps;  /* holds dps function pointers */

#endif /* ORTE_DPS_H */
