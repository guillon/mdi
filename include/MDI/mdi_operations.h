/*
 * Machine Description Interface Operations C API
 *
 * This software is delivered under the terms of the MIT License
 *
 * Copyright (c) 2016 STMicroelectronics
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 */

/**
 *
 * @file mdi_operations.h
 *
 * @brief Machine Description Interface Operations, C abstract interface.
 *
 * The Operations interface provides access to executable architecture
 * Operators as defined in the MDI interface.
 *
 * An executable Operator is abstracvted into an Operation instance from
 * which a Encoders, Disassemblers, Executions can work on.
 * Operations are created from Decoders and Assemblers.
 *
 */

#ifndef _MDI_OPERATIONS_H_INCLUDED
#define _MDI_OPERATIONS_H_INCLUDED

#include <MDI/mdi.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup MDI_Operation Operation object
 *
 * An abstract object for Operation, an executable
 * Operator instance.
 */
/**@{*/
/**
 * @brief Operation object abstraction
 *
 * An Operation is an executable instance of a MDI
 * Operator.
 * Decoders create new Operations from binary buffers.
 * Assemblers create new Operations from assembly buffers.
 * Disassemblers output assembly forms of Operations,
 * Executions execute Operations,
 * Encoders output encoded Operations into binary buffers.
 */
typedef MDI_object_t MDI_Operation_t;
/**@}*/

/**
 * @defgroup MDI_DecodeInfo Decode location information
 *
 * Decode location information for decoded Operations.
 */
/**@{*/

/**
 * @brief DecodeInfo object abstraction
 *
 * When an Operation was created by a Decoder for instance,
 * the Decoder may attach a DecodeInfo object to the Operation
 * such that other clients may use this information.
 * For instance a Disassembler may output the origin address of
 * the Operation when the DecodeInfo object is available.
 */
typedef MDI_object_t MDI_DecodeInfo_t;
/**@}*/

/**
 * @defgroup MDI_Decoder Decoder object
 *
 * An abstract object for decoding buffers into Operations.
 */
/**@{*/
/**
 * @brief Decoder object abstraction
 *
 * All decoding operation, i.e. construction of Operations
 * from encoded execution buffers are handled through this
 * object.
 */
typedef MDI_object_t MDI_Decoder_t;
/**@}*/

/**
 * @defgroup MDI_Disassembler Disassembler object
 *
 * An abstract object for disassembling Operations into assembly buffers.
 */
/**@{*/
/**
 * @brief Disassembler object abstraction
 *
 * All assembling operations, i.e. construction of assembly buffers
 * from Operation objects are handled through this object.
 */
typedef MDI_object_t MDI_Disassembler_t;
/**@}*/

/**
 * @defgroup MDI_Execution Execution context object
 *
 * An abstract object for executing Operations.
 */
/**@{*/
/**
 * @brief Execution object abstraction
 *
 * All operations executions, i.e. state change on a Processor and
 * associated peripherals by the execution of an Operation are
 * handled through this object.
 */
typedef MDI_object_t MDI_Execution_t;
/**@}*/

/**
 * @addtogroup MDI_Operation
 */
/**@{*/

/**
 * @brief Create a new Operation
 *
 * Create a new operation for the given opcode and operands list.
 *
 * @param self_ref A reference to the Operation object to construct.
 * @param operator The Operation MDI Operator.
 * @param opcount The Operation number of operands.
 * @param operands The pointer to the Abstract Operands buffer.
 * @param params Implementation defined parameters.
 * @return 0 on success, failure otherwise.
 */
MDI_INTERFACE MDI_res_t MDI_Operation_init(MDI_Operation_t *self_ref, MDI_Operator_t operator, MDI_size_t opcount, MDI_ptr_t operands, MDI_object_t params);

/**
 * @brief Destroy an Operation instance
 *
 * Destructor for an Operation instance.
 *
 * @param self_ref A reference to a valid Operation instance.
 * @return 0 on success, failure otherwise.
 */
MDI_INTERFACE MDI_res_t MDI_Operation_fini(MDI_Operation_t *self_ref);

/**
 * @brief MDI Operator for an Operation
 *
 * Get the MDI Operator associated to the Operation instance.
 *
 * @param self The Operation.
 * @return The MDI Operator.
 */
MDI_INTERFACE MDI_Operator_t MDI_Operation_operator(MDI_Operation_t self);

/**
 * @brief Operands count for an Operation
 *
 * Get the actual operands count for an operation.
 *
 * @param self The Operation.
 * @return The number of operands.
 */
MDI_INTERFACE MDI_size_t MDI_Operation_opcount(MDI_Operation_t self);

/**
 * @brief Abstract pointer to operands of an operation.
 *
 * Get the pointer to the Operation's operands. Implementation defined.
 *
 * @param self The Operation.
 * @return The operands pointer.
 */
MDI_INTERFACE MDI_ptr_t MDI_Operation_operands(MDI_Operation_t self);

/**
 * @brief Abstract pointer to decode information
 *
 * Abstract DecodeInfo object for the Operation is avazilable.
 *
 * @param self The Operation.
 * @return The DecodeInfo object or NULL.
 */
MDI_INTERFACE MDI_DecodeInfo_t MDI_Operation_decode_info(MDI_Operation_t self);

/**
 * @brief Set abstract decode information
 *
 * Set the DecodeInfo object for the Operation.
 *
 * @param self The Operation.
 * @param decode_info The DecodeInfo object.
 */
MDI_INTERFACE void MDI_Operation_set_decode_info(MDI_Operation_t self, MDI_DecodeInfo_t decode_info);
/**@}*/

/**
 * @addtogroup MDI_DecodeInfo
 */
/**@{*/

/**
 * @brief Create a new DecodeInfo
 *
 * Create a new DecodeInfo instance.
 *
 * @param self_ref A reference to the DecodeInfo object to construct.
 * @param buffer The decode buffer address.
 * @param offset The offset in the decode buffer.
 * @param size The size of the encoded Operation at the decoded offset.
 * @param params Implementation defined parameters.
 * @return 0 on success, failure otherwise.
 */
MDI_INTERFACE MDI_res_t MDI_DecodeInfo_init(MDI_DecodeInfo_t *self_ref, MDI_ptr_t buffer, MDI_size_t offset, MDI_size_t size, MDI_object_t params);

/**
 * @brief Destroy a DecodeInfo instance
 *
 * Destructor for an DecodeInfo instance.
 *
 * @param self_ref A reference to a valid DecodeInfo instance.
 * @return 0 on success, failure otherwise.
 */
MDI_INTERFACE MDI_res_t MDI_DecodeInfo_fini(MDI_DecodeInfo_t *self_ref);

/**
 * @brief Decode buffer
 *
 * Get the origin decode buffer address for the DecodeInfo.
 * Note that this buffer address may not be a valid address to
 * dereference, it may though be used as an information in 
 * Disassemblers outputs for instance.
 * 
 * @param self The DecodeInfo.
 * @return The origin decoding buffer.
 */
MDI_INTERFACE MDI_ptr_t MDI_DecodeInfo_buffer(MDI_DecodeInfo_t self);

/**
 * @brief Decode offset
 *
 * Get the origin byte offset into the origin decode buffer.
 *
 * @param self The DecodeInfo.
 * @return The origin decoding offset.
 */
MDI_INTERFACE MDI_size_t MDI_DecodeInfo_offset(MDI_DecodeInfo_t self);

/**
 * @brief Decode size
 *
 * Get the encoded size for the decoded Operation.
 *
 * @param self The DecodeInfo.
 * @return The encoded size.
 */
MDI_INTERFACE MDI_size_t MDI_DecodeInfo_size(MDI_DecodeInfo_t self);
/**@}*/

/**
 * @addtogroup MDI_Decoder
 */
/**@{*/

/**
 * @brief Create a new Decoder
 *
 * Create a new decoder instance for the given machine description
 * and the processor.
 *
 * @param self_ref A reference to the Decoder object to construct.
 * @param mdi A MDI machine description instance
 * @param processor The Processor to decode for.
 * @param params Implementation defined parameters.
 * @return 0 on success, failure otherwise.
 */
MDI_INTERFACE MDI_res_t MDI_Decoder_init(MDI_Decoder_t *self_ref, MDI_interface_t mdi, MDI_Processor_t processor, MDI_object_t params);

/**
 * @brief Destroy a Decoder instance
 *
 * Destructor for a Decoder instance.
 *
 * @param self_ref A reference to a valid Decoder instance.
 * @return 0 on success, failure otherwise.
 */
MDI_INTERFACE MDI_res_t MDI_Decoder_fini(MDI_Decoder_t *self_ref);

/**
 * @brief Decode a buffer into an Operation
 *
 * Given the buffer and buffer_size to an encoded stream, decode
 * current_ptr into an Operation and update current_ptr.
 *
 * The current_ptr reference must point to a position inside the
 * given buffer. It will not be updated if the decode point is an invalid
 * Opcode. Otherwise if a valid Opcode if found, the decoder will
 * construct the associated Operation and update the current_ptr reference
 * to the next decoding point.
 *
 * @param self A Decoder.
 * @param buffer An encoded buffer pointer.
 * @param buffer_size The size of the encoded buffer.
 * @param current_ptr The reference to the current decoding pointer.
 * @return A new Operation instance or NULL on invalid Opcode..
 */
MDI_INTERFACE MDI_Operation_t MDI_Decoder_decode(MDI_Decoder_t self, MDI_ptr_t buffer, MDI_size_t buffer_size, MDI_ptr_t *current_ptr);
/**@}*/

/**
 * @addtogroup MDI_Disassembler
 */
/**@{*/

/**
 * @brief Create a new Disassembler
 *
 * Create a new disassembler instance for the given machine description
 * and the processor.
 *
 * @param self_ref A reference to the Disassembler object to construct.
 * @param mdi A MDI machine description instance
 * @param processor The Processor to disassembler for.
 * @param params Implementaion defined parameters.
 * @return 0 on success, failure otherwise.
 */
MDI_INTERFACE MDI_res_t MDI_Disassembler_init(MDI_Disassembler_t *self_ref, MDI_interface_t mdi, MDI_Processor_t processor, MDI_object_t params);

/**
 * @brief Destroy a Disassembler instance
 *
 * Destructor for a Disassembler instance.
 *
 * @param self_ref A reference to a valid Disassembler instance.
 * @return 0 on success, failure otherwise.
 */
MDI_INTERFACE MDI_res_t MDI_Disassembler_fini(MDI_Disassembler_t *self_ref);

/**
 * @brief Disassemble an Operation into an assembly buffer
 *
 * The Operation is disassembled at the location referenced by current_ptr
 * inside the given disassembly buffer of size buffer_size.
 * The returned value is the size in bytes necessary for the disassembly, if
 * the output has been truncated, the returned size is greater or equal to
 * the maximum usable size. I.e. the value of current_ptr on entry plus the
 * returned size if greater or equal to the buffer limit (buffer + buffer_size).
 * In this case also the returned current_ptr will point to the buffer limit
 * (buffer + buffer_size). The emitted bytes are always '\0' terminated even
 * when truncated.
 *
 * @param self the Disassembler
 * @param operation The operation to disassemble.
 * @param buffer The Disassembly buffer.
 * @param buffer_size The disassembky buffer_size.
 * @param current_ptr The current pointer into the disassembly buffer.
 * @return The number of bytes necessary, may be larger than the emitted bytes.
 */
MDI_INTERFACE MDI_size_t MDI_Disassembler_disassemble(MDI_Disassembler_t self, MDI_Operation_t operation, MDI_ptr_mut_t buffer, MDI_size_t buffer_size, MDI_ptr_mut_t *current_ptr);
/**@}*/

/**
 * @addtogroup MDI_Execution
 */
/**@{*/

/**
 * @brief Create a new Execution context
 *
 * Create a new execution instance for the given machine description
 * and processor.
 *
 * @param self_ref A reference to the Execution object to construct.
 * @param mdi A MDI machine description instance
 * @param processor The Processor to decode for.
 * @param params Implementation defined parameters.
 * @return 0 on success, failure otherwise.
 */
MDI_INTERFACE MDI_res_t MDI_Execution_init(MDI_Execution_t *self_ref, MDI_interface_t mdi, MDI_Processor_t processor, MDI_object_t params);

/**
 * @brief Destroy an Execution context
 *
 * Destruct a valid Execution object.
 *
 * @param self_ref A reference to a valid Execution object.
 * @return 0 on success, failure otherwise.
 */
MDI_INTERFACE MDI_res_t MDI_Execution_fini(MDI_Execution_t *self_ref);

/**
 * @brief Execute an Operation
 *
 * Execute an Operation, changing the current Execution state.
 *
 * @param self An Execution context.
 * @param operation The Operation to execute.
 * @return 0 on success, >0 for requesting more Operation, <0 on failure.
 */
MDI_INTERFACE MDI_res_t MDI_Execution_execute(MDI_Execution_t self, MDI_Operation_t operation);

/**
 * @brief Execution context current Program Counter
 *
 * Helper to get the canonical PC value for the current
 * Execution context if available.
 * Implementation defined if the Program Counter is not
 * available.
 *
 * @param self An Execution context.
 * @return The current abstract Program Counter value.
 */
MDI_INTERFACE MDI_size_t MDI_Execution_pc(MDI_Execution_t self);

/**
 * @brief Execution context current return value
 *
 * Helper to get the ABI defined first return value if
 * available.
 * Implementation defined if the ABI return value is not
 * available.
 *
 * @param self An Execution context.
 * @return The current abstract first return value.
 */
MDI_INTERFACE uint64_t MDI_Execution_ret0(MDI_Execution_t self);

/**
 * @brief Pre execution step debug helper
 *
 * Helper called before execution of an Operation.
 * The implementation should be empty, this is used as a helper
 * for adding debugger breakpoints before execution.
 *
 * @param self An Execution context
 */
MDI_INTERFACE void MDI_Execution_stepin(MDI_Execution_t self);

/**
 * @brief Post execution step debug helper
 *
 * Helper called after execution of an Operation.
 * The implementation should be empty, this is used as a helper
 * for adding debugger breakpoints after execution.
 *
 * @param self An Execution context
 */
MDI_INTERFACE void MDI_Execution_stepout(MDI_Execution_t self);
/**@}*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _MDI_OPERATIONS_H_INCLUDED */

