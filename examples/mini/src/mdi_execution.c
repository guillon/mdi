/*
 * Execution Interface Implementation for MINI platform.
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

#define RF_R32_COUNT 32
#define RF_PC_COUNT 1
#define RF_SAVE_COUNT 6
#define RF_STAT_COUNT 1
#define RF_TRAP_COUNT 16
#define MEM_BYTES 4096

typedef struct {
    uint32_t R32[RF_R32_COUNT];
    uint32_t PC[RF_PC_COUNT];
    uint32_t SAVE[RF_SAVE_COUNT];
    uint32_t STAT[RF_STAT_COUNT];
    uint32_t TRAP[RF_TRAP_COUNT];
} mini_cpu_t;

typedef char *mini_memory_t;

typedef struct {
    MDI_interface_t interface;
    MDI_Processor_t processor;
    mini_cpu_t cpu;
    mini_memory_t mem;
} execution_context_t;

#define EXE_CTX_CPU(ctx) &(ctx->cpu)
#define EXE_CTX_MEM(ctx) &(ctx->mem)
#define EXE_CPU_CLONE(cpu_dst,cpu_src) (cpu_dst) = *(cpu_src)
#define EXE_MEM_CLONE(mem_dst,mem_src) (mem_dst) = *(mem_src)
#define EXE_CPU_UPDATE(cpu_dst,cpu_src) (cpu_dst) = *(cpu_src)
#define EXE_MEM_UPDATE(mem_dst,mem_src) (mem_dst) = *(mem_src)

#define EXE_CPU_RR(cpu,rf,idx) ((uint32_t)(cpu.rf[idx]))
#define EXE_CPU_RS(cpu,rf,idx) *((uint32_t *)(&cpu.rf[idx]))
#define EXE_MEM_FETCH32(mem,idx) (*((uint32_t *)(&mem[idx])))
#define EXE_MEM_SLICE32(mem,idx) *((uint32_t *)(&mem[idx]))
#define EXE_OPS(operands,idx) ((uint32_t)operands[idx])
#define EXE_CTX_T execution_context_t *
#define EXE_OPS_T const intptr_t *
#define CPU_T mini_cpu_t
#define MEM_T mini_memory_t

#include "generated_executions.inc"

#define UNUSED(var) (void)(var)

MDI_res_t MDI_Execution_init(MDI_Execution_t *self_ref, MDI_interface_t mdi, MDI_Processor_t processor,
                             MDI_object_t params)
{
    execution_context_t *context;

    assert(self_ref != NULL);
    UNUSED(params);

    context = (execution_context_t *)calloc(1, sizeof(execution_context_t));
    context->mem = (char *)calloc(MEM_BYTES, sizeof(char));
    context->interface = mdi;
    context->processor = processor;

    *self_ref = (MDI_Execution_t)context;
    
    return 0;
}

MDI_res_t MDI_Execution_fini(MDI_Execution_t *self_ref)
{
    execution_context_t *context;

    assert(self_ref != NULL);
    context = (execution_context_t *)*self_ref;
    if (context == NULL) return -1;

    free(context->mem);
    context->mem = NULL;
    free(context);
    *self_ref = NULL;
    
    return 0;
}

MDI_size_t MDI_Execution_pc(MDI_Execution_t execution)
{
    execution_context_t *context;

    assert(execution != NULL);
    context = (execution_context_t *)execution;

    return (MDI_size_t)context->cpu.PC[0];
}

uint64_t MDI_Execution_ret0(MDI_Execution_t execution)
{
    execution_context_t *context;

    assert(execution != NULL);
    context = (execution_context_t *)execution;

    return (uint64_t)context->cpu.R32[0];
}

void MDI_Execution_stepin(MDI_Execution_t execution)
{
}

void MDI_Execution_stepout(MDI_Execution_t execution)
{
}

MDI_res_t MDI_Execution_execute(MDI_Execution_t self, MDI_Operation_t operation)
{
    execution_context_t *execution;
    size_t op_size;
    int res;
    const intptr_t *exec_operands;
    MDI_Opcode_t opcode;
    MDI_Operator_t operator;
    MDI_Processor_t processor;
    MDI_DecodeInfo_t decode_info;
    size_t opcode_idx;

    assert(self != NULL);
    execution = (execution_context_t *)self;
    assert(operation != NULL);

    processor = execution->processor;
    operator = MDI_Operation_operator(operation);
    opcode = MDI_Operator_opcode(operator, processor);
    opcode_idx = (size_t)(intptr_t)opcode;
    decode_info = MDI_Operation_decode_info(operation);
    assert(decode_info != NULL);
    op_size = MDI_DecodeInfo_size(decode_info);

    exec_operands = (const intptr_t *)MDI_Operation_operands(operation);

    MDI_Execution_stepin(self);
    res = _executions[opcode_idx](execution, exec_operands, op_size);
    MDI_Execution_stepout(self);

    return res;
}
