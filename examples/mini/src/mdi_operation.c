/*
 * Operation Interface Implementation for MINI platform.
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
#include <assert.h>
#include <MDI/mdi.h>
#include <MDI/mdi_operations.h>

#define UNUSED(var) ((void)(var))

typedef struct {
    intptr_t buffer;
    size_t offset;
    size_t size;
} decode_info_t;

typedef struct {
    const char *filename;
    size_t line;
    size_t col;
    size_t size;
} assemble_info_t;

typedef struct {
    uint32_t operator;
    uint32_t opcount;
    intptr_t *operands;
    uint32_t opsize;
    decode_info_t *decode_info;
    assemble_info_t *parse_info;
} operation_t;

MDI_res_t MDI_Operation_init(MDI_Operation_t *self_ref, MDI_Operator_t operator, MDI_size_t opcount, MDI_ptr_t operands, MDI_object_t params)
{
    operation_t *operation;
    int i;
    assert(self_ref != NULL);
    UNUSED(params);
    operation = (operation_t *)calloc(1, sizeof(operation_t));
    operation->operator = (uint32_t)(intptr_t)operator;
    operation->opcount = opcount;
    if (opcount > 0) {
        operation->operands = (intptr_t *)calloc(opcount, sizeof(intptr_t));
    }

    for (i = 0; i < opcount; i++) {
        operation->operands[i] = ((intptr_t *)operands)[i];
    }
    *self_ref = operation;
    return 0;
}

MDI_res_t MDI_Operation_fini(MDI_Operation_t *self_ref)
{
    operation_t *operation;

    assert(self_ref != NULL);
    operation = (operation_t *)*self_ref;
    if (operation == NULL) return -1;

    if (operation->opcount > 0) {
        free(operation->operands);
        operation->operands = NULL;
    }
    free(operation);
    *self_ref = NULL;
    return 0;
}

MDI_Operator_t MDI_Operation_operator(MDI_Operation_t self)
{
    operation_t *operation;

    assert(self != NULL);
    operation = (operation_t *)self;
    
    return (MDI_Operator_t)(intptr_t)operation->operator;
}

MDI_ptr_t MDI_Operation_operands(MDI_Operation_t self)
{
    operation_t *operation;

    assert(self != NULL);
    operation = (operation_t *)self;
    
    return (MDI_ptr_t)operation->operands;
}

MDI_size_t MDI_Operation_opcount(MDI_Operation_t self)
{
    operation_t *operation;

    assert(self != NULL);
    operation = (operation_t *)self;
    
    return (MDI_size_t)operation->opcount;
}

MDI_DecodeInfo_t MDI_Operation_decode_info(MDI_Operation_t self)
{
    operation_t *operation;

    assert(self != NULL);
    operation = (operation_t *)self;
    
    return (MDI_DecodeInfo_t)operation->decode_info;
}

void MDI_Operation_set_decode_info(MDI_Operation_t self, MDI_DecodeInfo_t decode_info)
{
    operation_t *operation;

    assert(self != NULL);
    operation = (operation_t *)self;
    
    operation->decode_info = (decode_info_t *)decode_info;
}

MDI_res_t MDI_DecodeInfo_init(MDI_DecodeInfo_t *self_ref, MDI_ptr_t buffer, MDI_size_t offset, MDI_size_t size, MDI_object_t params)
{
    decode_info_t *decode_info;

    assert(self_ref != NULL);
    UNUSED(params);

    decode_info = (decode_info_t *)calloc(1, sizeof(decode_info_t));
    decode_info->buffer = (intptr_t)buffer;
    decode_info->offset = offset;
    decode_info->size = size;

    *self_ref = decode_info;
    return 0;
}

MDI_INTERFACE MDI_res_t MDI_DecodeInfo_fini(MDI_DecodeInfo_t *self_ref)
{
    decode_info_t *decode_info;

    assert(self_ref != NULL);
    decode_info = (decode_info_t *)*self_ref;
    if (decode_info == NULL) return -1;
    
    free(decode_info);
    *self_ref = NULL;
    return 0;
}

MDI_ptr_t MDI_DecodeInfo_buffer(MDI_DecodeInfo_t self)
{
    decode_info_t *decode_info;

    assert(self != NULL);
    decode_info = (decode_info_t *)self;
    
    return (MDI_ptr_t)decode_info->buffer;
}

MDI_size_t MDI_DecodeInfo_offset(MDI_DecodeInfo_t self)
{
    decode_info_t *decode_info;

    assert(self != NULL);
    decode_info = (decode_info_t *)self;
    
    return (MDI_size_t)decode_info->offset;
}

MDI_size_t MDI_DecodeInfo_size(MDI_DecodeInfo_t self)
{
    decode_info_t *decode_info;

    assert(self != NULL);
    decode_info = (decode_info_t *)self;
    
    return (MDI_size_t)decode_info->size;
}

