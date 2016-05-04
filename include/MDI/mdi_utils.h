#ifndef _MDI_UTILS_H_INCLUDED
#define _MDI_UTILS_H_INCLUDED

#include <MDI/mdi.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * MDI utils object instance types.
 */
typedef MDI_object_t MDI_Operation_t;
typedef MDI_object_t MDI_Execution_t;

/*
 * Decoder.abstract interface.
 */
MDI_INTERFACE MDI_Operation_t MDI_Operation_decode(MDI_interface_t self, MDI_ptr_t buffer, MDI_size_t buffer_size, MDI_ptr_t *current_ptr);

/*
 * Operation output.
 */
MDI_INTERFACE MDI_size_t MDI_Operation_print(MDI_interface_t self, MDI_Operation_t operation, MDI_ptr_mut_t buffer, MDI_size_t buffer_size, MDI_ptr_mut_t *current_ptr);

/*
 * Operation execution
 */
MDI_INTERFACE MDI_res_t MDI_Execution_init(MDI_Execution_t *execution, MDI_object_t params);
MDI_INTERFACE MDI_res_t MDI_Execution_fini(MDI_Execution_t *execution);
MDI_INTERFACE MDI_size_t MDI_Execution_pc(MDI_Execution_t execution);
MDI_INTERFACE uint64_t MDI_Execution_ret0(MDI_Execution_t execution);
MDI_INTERFACE void MDI_Execution_stepin(MDI_Execution_t execution);
MDI_INTERFACE void MDI_Execution_stepout(MDI_Execution_t execution);

MDI_INTERFACE MDI_res_t MDI_Operation_execute(MDI_Execution_t execution, MDI_Operation_t operation);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _MDI_UTILS_H_INCLUDED */

