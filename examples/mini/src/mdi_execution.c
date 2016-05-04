#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <MDI/mdi.h>
#include <MDI/mdi_utils.h>

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
#define EXE_OPS(operands,idx) (operands[idx])
#define EXE_CTX_T execution_context_t *
#define EXE_OPS_T const uint32_t *
#define CPU_T mini_cpu_t
#define MEM_T mini_memory_t

#include "generated_executions.inc"

#define UNUSED(var) (void)(var)

MDI_res_t MDI_Execution_init(MDI_Execution_t *execution, MDI_object_t params)
{
    execution_context_t *context;

    assert(execution != NULL);
    UNUSED(params);

    context = (execution_context_t *)calloc(1, sizeof(execution_context_t));
    context->mem = (char *)calloc(MEM_BYTES, sizeof(char));

    *execution = (MDI_Execution_t)context;
    
    return 0;
}

MDI_res_t MDI_Execution_fini(MDI_Execution_t *execution)
{
    execution_context_t *context;

    assert(execution != NULL);
    context = (execution_context_t *)*execution;

    free(context->mem);
    context->mem = NULL;
    free(context);
    *execution = NULL;
    
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

#define OPERANDS_MAX 4
typedef struct {
    uint32_t opcode;
    uint32_t params[OPERANDS_MAX];
    uint32_t opsize;
} operation_t;

MDI_res_t MDI_Operation_execute(MDI_Execution_t execution, MDI_Operation_t operation)
{
    operation_t *operation_ptr;
    execution_context_t *context;
    uint32_t *params;
    uint32_t opcode;
    uint32_t op_size;
    int res;

    assert(execution != NULL);
    context = (execution_context_t *)execution;
    assert(operation != NULL);

    operation_ptr = (operation_t *)operation;
    opcode = operation_ptr->opcode;
    op_size = operation_ptr->opsize;
    params = &operation_ptr->params[0];

    MDI_Execution_stepin(execution);
    res = _executions[opcode](context, params, op_size);
    MDI_Execution_stepout(execution);

    return res;
}
