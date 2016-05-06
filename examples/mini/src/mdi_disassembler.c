/*
 * Disassembler Interface Implementation for MINI platform.
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

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <MDI/mdi.h>
#include <MDI/mdi_operations.h>

#define MAX_OPERANDS 4

typedef struct {
    MDI_interface_t mdi;
    MDI_Processor_t processor;
} disassembler_t;

MDI_res_t MDI_Disassembler_init(MDI_Disassembler_t *self_ref, MDI_interface_t mdi, MDI_Processor_t processor, MDI_object_t params)
{
    disassembler_t *disassembler;

    assert(self_ref != NULL);

    disassembler = (disassembler_t *)calloc(1, sizeof(disassembler_t));
    disassembler->mdi = mdi;
    disassembler->processor = processor;
    
    *self_ref = (MDI_Disassembler_t)disassembler;
    return 0;
}

MDI_res_t MDI_Disassembler_fini(MDI_Disassembler_t *self_ref)
{
    disassembler_t *disassembler;

    assert(self_ref != NULL);
    disassembler = (disassembler_t *)*self_ref;
    if (disassembler == NULL) return -1;

    free(disassembler);
    *self_ref = NULL;

    return 0;
}

MDI_size_t MDI_Disassembler_disassemble(MDI_Disassembler_t self, MDI_Operation_t operation, MDI_ptr_mut_t buffer, MDI_size_t buffer_size, MDI_ptr_mut_t *current_ptr)
{
    char *current;
    char *limit;
    size_t offset;
    size_t nbytes;
    int size;
    uint32_t parse_operands[MAX_OPERANDS];
    disassembler_t *disassembler;
    MDI_Operator_t operator;
    MDI_Processor_t processor;
    MDI_Opcode_t opcode;
    MDI_str_t parsing;
    MDI_DecodeInfo_t decode_info;
    int i;

    assert(self != NULL);
    assert(operation != NULL);
    assert(buffer != NULL);
    assert(current_ptr != NULL);
    assert((const char *)*current_ptr >= (const char *)buffer && (const char *)*current_ptr < (const char *)buffer + buffer_size);

    current = *current_ptr;
    limit = buffer + buffer_size;
    offset = current - buffer;
    nbytes = 0;

    disassembler = (disassembler_t *)self;
    processor = disassembler->processor;
    operator = MDI_Operation_operator(operation);
    opcode = MDI_Operator_opcode(operator, processor);
    parsing = MDI_Opcode_parsing(opcode);

    /* Check that we don't have more operands than supported by this disassembler implementation. */
    /* This implementation only manage 32 bits operands. */
    assert(MDI_Operation_opcount(operation) <= MAX_OPERANDS);
    for (i = 0; i < MDI_Operation_opcount(operation); i++) {
        parse_operands[i] = (uint32_t)((intptr_t *)MDI_Operation_operands(operation))[i];
    }

    size = snprintf(current, limit - current, "L%"PRIuPTR":\t", offset);
    nbytes += size;
    if (size >= limit - current) current = limit;
    else current += size;

    assert(MAX_OPERANDS == 4);
    size = snprintf(current, limit - current, parsing,
                    MDI_Instruction_mnemonic(MDI_Opcode_instruction(opcode)),
                    parse_operands[0], parse_operands[1],
                    parse_operands[2], parse_operands[3]);
    nbytes += size;
    if (size >= limit - current) current = limit;
    else current += size;

    decode_info = MDI_Operation_decode_info(operation);
    if (decode_info) {
        size = snprintf(current, limit - current, "\t\t//@0x%"PRIxPTR" + [%"PRIuPTR", %"PRIuPTR")",
                        (intptr_t)MDI_DecodeInfo_buffer(decode_info),
                        (intptr_t)MDI_DecodeInfo_offset(decode_info),
                        (intptr_t)(MDI_DecodeInfo_offset(decode_info)+MDI_DecodeInfo_size(decode_info)));
        if (size >= limit - current) current = limit;
        else current += size;
    }

    if (nbytes >= buffer_size)
        buffer[buffer_size-1] = '\0';

    *current_ptr = current;
    return nbytes;
}

