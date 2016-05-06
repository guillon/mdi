/*
 * Decoder Interface Implementation for MINI platform.
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
#include <ctype.h>
#include <string.h>
#include <MDI/mdi.h>
#include <MDI/mdi_operations.h>

#define OPERANDS_MAX 4

typedef struct {
    MDI_interface_t mdi;
    MDI_Processor_t processor;
} decoder_t;


MDI_res_t MDI_Decoder_init(MDI_Decoder_t *self_ref, MDI_interface_t mdi, MDI_Processor_t processor, MDI_object_t params)
{
    decoder_t *decoder;

    assert(self_ref != NULL);

    decoder = (decoder_t *)calloc(1, sizeof(decoder_t));
    decoder->mdi = mdi;
    decoder->processor = processor;
    
    *self_ref = (MDI_Decoder_t)decoder;
    return 0;
}

MDI_res_t MDI_Decoder_fini(MDI_Decoder_t *self_ref)
{
    decoder_t *decoder;

    assert(self_ref != NULL);
    decoder = (decoder_t *)*self_ref;
    if (decoder == NULL) return -1;

    free(decoder);
    *self_ref = NULL;

    return 0;
}


MDI_Operation_t MDI_Decoder_decode(MDI_Decoder_t self, MDI_ptr_t buffer, MDI_size_t buffer_size, MDI_ptr_t *current_ptr)
{
    decoder_t *decoder;
    const char *start, *limit, *current;
    const char *opcode, *opcode_end, *operator_end;
    uint32_t decode_operands[OPERANDS_MAX];
    intptr_t operation_operands[OPERANDS_MAX];
    MDI_Operation_t operation;
    MDI_Operator_t operator;
    MDI_str_t encoding;
    MDI_interface_t interface;
    MDI_Processor_t processor;
    MDI_DecodeInfo_t decode_info;
    MDI_res_t res;
    int num_operands;
    int i;

    assert(self != NULL);
    assert(buffer != NULL);
    assert(current_ptr != NULL);
    assert((const char *)*current_ptr >= (const char *)buffer && (const char *)*current_ptr < (const char *)buffer + buffer_size);

    decoder = (decoder_t *)self;
    interface = decoder->mdi;
    processor = decoder->processor;

    limit = (const char *)buffer + buffer_size;
    start = (const char *)*current_ptr;
    current = start;

    /* Skip leading spaces. */
    while(current < limit && isspace(*current))
        current++;
    opcode = current;

    /* Count parameters and find end of opcode field. */
    while (current < limit && *current != '.' && *current != '/') {
        current++;
    }
    operator_end = current;

    while (current < limit && *current != '.') {
        current++;
    }
    opcode_end = current;

    /* No opcode. */
    if (current == limit) return (MDI_Operation_t)NULL;

    /* Operator match for the given processor. Match done on Opcode. */
    for (i = 0; i < MDI_Operators_count(interface); i++) {
        operator = MDI_Operators_iter(interface, i);
        encoding = MDI_Opcode_encoding(MDI_Operator_opcode(operator, processor));
        if (strncmp(opcode, encoding, operator_end - opcode) == 0 &&
            (encoding[operator_end - opcode] == '.' ||
             encoding[operator_end - opcode] == '/'))
            break;
    }

    /* No encoding match for the Operator. */
    if (i == MDI_Operators_count(interface)) return (MDI_Operation_t)NULL;
    
    /* Parse operation operands. Assume a limit of 4 operands. */
    /* This implementation only manage 32 bits operands. */
    assert(OPERANDS_MAX == 4);
    num_operands = sscanf(opcode, encoding,
                          &decode_operands[0], &decode_operands[1],
                          &decode_operands[2], &decode_operands[3]);

    /* Skip trailing spaces. */
    current = opcode_end + 1;
    while(current < limit && isspace(*current))
        current++;

    for (i = 0; i < num_operands; i++) {
        operation_operands[i] = (intptr_t)decode_operands[i];
    }

    res = MDI_DecodeInfo_init(&decode_info, buffer,
                              start - buffer /* offset */,
                              current - start /* opsize */,
                              NULL);
    if (res != 0) return (MDI_Operation_t)NULL;

    res = MDI_Operation_init(&operation, operator, num_operands, (MDI_ptr_t)operation_operands, NULL);
    if (res != 0) {
        MDI_DecodeInfo_fini(&decode_info);
        return (MDI_Operation_t)NULL;
    }

    MDI_Operation_set_decode_info(operation, decode_info);

    *current_ptr = current;
    return operation;
}
