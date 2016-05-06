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
#include <MDI/mdi_operations.h>

/*
 * The implemented MDI revision.
 */
#define THIS_REVISION MDI_VERSION_MAKE_REV(0,2,0)
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

MDI_str_t MDI_Opcode_parsing(MDI_Opcode_t self)
{
    assert((intptr_t)self >= 0 && (intptr_t)self < INSTRUCTIONS_COUNT());
    return instructions[(intptr_t)self].parsing;
}

MDI_str_t MDI_Opcode_encoding(MDI_Opcode_t self)
{
    assert((intptr_t)self >= 0 && (intptr_t)self < INSTRUCTIONS_COUNT());
    return instructions[(intptr_t)self].encoding;
}

MDI_str_t MDI_Opcode_execution(MDI_Opcode_t self)
{
    assert((intptr_t)self >= 0 && (intptr_t)self < INSTRUCTIONS_COUNT());
    return instructions[(intptr_t)self].execution;
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

