/*
 * Machine Description Interface Implementation for MINI platform.
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

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <MDI/mdi.h>
#include <MDI/mdi_utils.h>

/*
 * The implemented MDI revision.
 */
#define THIS_REVISION MDI_VERSION_MAKE_REV(0,1,0)
static const int mdi_revision = THIS_REVISION;

#if THIS_REVISION != MDI_VERSION_REV
#error "Compiling with incompatible MDI interface revision"
#endif

#define UNUSED(var) (void)(var) /*Explicitly marked unused var*/

/*
 * There is not state, thus the interface object is
 * just a singleton pointer.
 */
static const int *mdi_interface = &mdi_revision;

MDI_res_t MDI_interface_init(MDI_interface_t *self_ref, MDI_object_t params)
{
    UNUSED(params),
    *self_ref = (MDI_interface_t)mdi_interface;
    return 0;
}

MDI_res_t MDI_interface_fini(MDI_interface_t *self_ref)
{
    assert(self_ref != NULL);
    *self_ref = NULL;
    return 0;
}

MDI_rev_t MDI_interface_revision(MDI_interface_t self)
{
    assert(self != 0);
    return (MDI_rev_t)(*(const int *)self);
}

/*
 * This mini proc implementation consists of a simple
 * enumeration of attributes defined in the enum
 * descriptions mde_mini_*.enum.
 * This example as a single encoding an a 1-1 mapping
 * between instructions and operators. Hence
 * the instructions/operators/opcodes atttributes are
 * all accessed though the same instruction index.
 */

typedef struct {
    const char *ID;
    const char *processors;
    const char *short_desc;
    const char *description;
} platform_t;

#define ENUM(ID,processors,short_desc,description) { ID, processors, short_desc, description },
static const platform_t platforms[1] = {
    #include "mde/platform.enum"
};
#undef ENUM

MDI_Platform_t MDI_Platform(MDI_interface_t self)
{
    assert(self == mdi_interface);
    UNUSED(self);
    return (MDI_Platform_t)(intptr_t)0;
}

MDI_str_t MDI_Platform_ID(MDI_Platform_t self)
{
    assert((intptr_t)self == 0);
    return (MDI_str_t)platforms[(intptr_t)self].ID;
}

MDI_str_t MDI_Platform_processors(MDI_Platform_t self)
{
    assert((intptr_t)self == 0);
    return (MDI_str_t)platforms[(intptr_t)self].processors;
}

typedef struct {
    const char *ID;
    const char *mnemonic;
    const char *properties;
    const char *parsing;
    const char *encoding;
    const char *short_desc;
    const char *execution;
    const char *description;
} instruction_t;

#define ENUM(ID,mnemonic,properties,parsing,encoding, short_desc, execution, description) { ID, mnemonic, properties, parsing, encoding, short_desc, execution, description },
static const instruction_t instructions [] = {
    #include "mde/instructions.enum"
};
#undef ENUM
#define INSTRUCTIONS_COUNT() (sizeof(instructions)/sizeof(*instructions))

/* Assume no more than this number of operands for an operation. */
#define OPERANDS_MAX 4
typedef struct {
    intptr_t buffer_addr;
    size_t buffer_offset;
} decode_info_t;

typedef struct {
    const char *filename;
    size_t line;
    size_t col;
} parse_info_t;

typedef struct {
    uint32_t opcode;
    uint32_t params[OPERANDS_MAX];
    uint32_t opsize;
    union {
        decode_info_t decode;
        parse_info_t parse;
    } info;
} operation_t;

MDI_idx_t MDI_Instructions_count(MDI_interface_t self)
{
    assert(self == mdi_interface);
    UNUSED(self);
    return (MDI_idx_t)INSTRUCTIONS_COUNT();
}

MDI_Instruction_t MDI_Instructions_iter(MDI_interface_t self, MDI_idx_t instruction_idx)
{
    assert(instruction_idx >= 0 && instruction_idx < INSTRUCTIONS_COUNT());
    UNUSED(self);
    return (MDI_Instruction_t)(intptr_t)instruction_idx;
}

MDI_idx_t MDI_Instruction_idx(MDI_Instruction_t self)
{
    assert((intptr_t)self >= 0 && (intptr_t)self < INSTRUCTIONS_COUNT());
    return (MDI_idx_t)(intptr_t)self;
}

MDI_str_t MDI_Instruction_ID(MDI_Instruction_t self)
{
    assert((intptr_t)self >= 0 && (intptr_t)self < INSTRUCTIONS_COUNT());
    return instructions[(intptr_t)self].ID;
}

MDI_str_t MDI_Instruction_mnemonic(MDI_Instruction_t self)
{
    assert((intptr_t)self >= 0 && (intptr_t)self < INSTRUCTIONS_COUNT());
    return instructions[(intptr_t)self].mnemonic;
}

MDI_str_t MDI_Instruction_properties(MDI_Instruction_t self)
{
    assert((intptr_t)self >= 0 && (intptr_t)self < INSTRUCTIONS_COUNT());
    return instructions[(intptr_t)self].properties;
}

MDI_idx_t MDI_Opcodes_count(MDI_interface_t self)
{
    assert(self == mdi_interface);
    UNUSED(self);
    return (MDI_idx_t)INSTRUCTIONS_COUNT();
}

MDI_Opcode_t MDI_Opcodes_iter(MDI_interface_t self, MDI_idx_t opcode_idx)
{
    assert(opcode_idx >= 0 && opcode_idx < INSTRUCTIONS_COUNT());
    UNUSED(self);
    return (MDI_Opcode_t)(intptr_t)opcode_idx;
}

MDI_idx_t MDI_Opcode_idx(MDI_Opcode_t self)
{
    assert((intptr_t)self >= 0 && (intptr_t)self < INSTRUCTIONS_COUNT());
    return (MDI_idx_t)(intptr_t)self;
}

MDI_str_t MDI_Opcode_ID(MDI_Opcode_t self)
{
    assert((intptr_t)self >= 0 && (intptr_t)self < INSTRUCTIONS_COUNT());
    return instructions[(intptr_t)self].ID;
}

MDI_Instruction_t MDI_Opcode_instruction(MDI_Opcode_t self)
{
    assert((intptr_t)self >= 0 && (intptr_t)self < INSTRUCTIONS_COUNT());
    return (MDI_Instruction_t)self;
}


MDI_idx_t MDI_Operators_count(MDI_interface_t self)
{
    assert(self == mdi_interface);
    UNUSED(self);
    return (MDI_idx_t)INSTRUCTIONS_COUNT();
}

MDI_Operator_t MDI_Operators_iter(MDI_interface_t self, MDI_idx_t operator_idx)
{
    assert(operator_idx >= 0 && operator_idx < INSTRUCTIONS_COUNT());
    UNUSED(self);
    return (MDI_Operator_t)(intptr_t)operator_idx;
}

MDI_idx_t MDI_Operator_idx(MDI_Operator_t self)
{
    assert((intptr_t)self >= 0 && (intptr_t)self < INSTRUCTIONS_COUNT());
    return (MDI_idx_t)(intptr_t)self;
}

MDI_str_t MDI_Operator_ID(MDI_Operator_t self)
{
    assert((intptr_t)self >= 0 && (intptr_t)self < INSTRUCTIONS_COUNT());
    return instructions[(intptr_t)self].ID;
}

MDI_Instruction_t MDI_Operator_opcode(MDI_Operator_t self, MDI_Processor_t processor)
{
    assert((intptr_t)processor == 0);
    assert((intptr_t)self >= 0 && (intptr_t)self < INSTRUCTIONS_COUNT());
    return (MDI_Instruction_t)self;
}

MDI_str_t MDI_Operator_attributes(MDI_Operator_t self)
{
    assert((intptr_t)self >= 0 && (intptr_t)self < INSTRUCTIONS_COUNT());
    return instructions[(intptr_t)self].properties;
}

MDI_Operation_t MDI_Operation_decode(MDI_interface_t self, MDI_ptr_t buffer, MDI_size_t buffer_size, MDI_ptr_t *current_ptr)
{
    const char *start, *limit, *current;
    const char *opcode, *opcode_end, *operator_end;
    uint32_t params[OPERANDS_MAX];
    int inst_idx;
    operation_t *operation;

    assert(buffer != NULL);
    assert(current_ptr != NULL);
    assert((const char *)*current_ptr >= (const char *)buffer && (const char *)*current_ptr < (const char *)buffer + buffer_size);

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

    /* Opcode match. */
    for (inst_idx = 0; inst_idx < INSTRUCTIONS_COUNT(); inst_idx++) {
        if (strncmp(opcode, instructions[inst_idx].encoding, operator_end - opcode) == 0 &&
            (instructions[inst_idx].encoding[operator_end - opcode] == '.' ||
             instructions[inst_idx].encoding[operator_end - opcode] == '/'))
            break;
    }

    /* No opcode match. */
    if (inst_idx == INSTRUCTIONS_COUNT()) return (MDI_Operation_t)NULL;
    
    /* Parse operation operands. Assume a limit of 4 operands. */
    assert(OPERANDS_MAX <= 4);
    params[0] = params[1] = params[2] = params[3] = 0;
    sscanf(opcode, instructions[inst_idx].encoding,
           params, params+1, params+2, params+3);

    /* Skip trailing spaces. */
    current = opcode_end + 1;
    while(current < limit && isspace(*current))
        current++;

    operation = (operation_t *)calloc(1,sizeof(operation_t));
    operation->opcode = inst_idx;
    operation->opsize = current - start;
    operation->params[0] = params[0];
    operation->params[1] = params[1];
    operation->params[2] = params[2];
    operation->params[3] = params[3];
    operation->info.decode.buffer_addr = (intptr_t)buffer;
    operation->info.decode.buffer_offset = start - buffer;

    *current_ptr = current;
    return (MDI_Operation_t)operation;
}

MDI_size_t MDI_Operation_print(MDI_interface_t self, MDI_Operation_t operation, MDI_ptr_mut_t buffer, MDI_size_t buffer_size, MDI_ptr_mut_t *current_ptr)
{
    operation_t *operation_ptr;
    char *current;
    char *limit;
    size_t offset;
    size_t nbytes;
    int size;
    uint32_t *params;
    size_t opcode;

    assert(operation != NULL);
    assert(buffer != NULL);
    assert(current_ptr != NULL);
    assert((const char *)*current_ptr >= (const char *)buffer && (const char *)*current_ptr < (const char *)buffer + buffer_size);

    operation_ptr = (operation_t *)operation;
    current = *current_ptr;
    limit = buffer + buffer_size;
    offset = current - buffer;
    params = &operation_ptr->params[0];
    opcode = operation_ptr->opcode;
    nbytes = 0;

    size = snprintf(current, limit - current, "L%"PRIuPTR":\t", offset);
    nbytes += size;
    if (size >= limit - current) current = limit;
    else current += size;

    size = snprintf(current, limit - current, instructions[opcode].parsing,
                    instructions[opcode].mnemonic,
                    params[0], params[1], params[2], params[3]);
    nbytes += size;
    if (size >= limit - current) current = limit;
    else current += size;

    size = snprintf(current, limit - current, "\t\t//@0x%"PRIxPTR,
                    operation_ptr->info.decode.buffer_addr +
                    operation_ptr->info.decode.buffer_offset);
    if (size >= limit - current) current = limit;
    else current += size;

    if (nbytes >= buffer_size)
        buffer[buffer_size-1] = '\0';

    *current_ptr = current;
    return nbytes;
}
