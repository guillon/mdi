/*
 * Machine Description Interface C API
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

#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <MDI/mdi.h>
#include <MDI/mdi_operations.h>

static int verbose = 1;

int execute(MDI_interface_t interface, const char *input_fname)
{
    static char buffer[4096];
    int rcode = -1;
    FILE *input;
    size_t nbytes;
    MDI_Execution_t execution = NULL;
    MDI_Decoder_t decoder = NULL;
    MDI_Disassembler_t disassembler = NULL;
    MDI_res_t res;
    MDI_size_t stop_pc, next_pc;
    uint64_t count = 0;

    if (strcmp(input_fname, "-") == 0) {
        input = stdin;
    } else {
        input = fopen(input_fname, "rb");
        if (input == NULL) {
            fprintf(stderr, "error opening %s: ", input_fname);
            perror("");
            goto end_of_execute;
        }
    }

    nbytes = fread(buffer, 1, sizeof(buffer), input);
    if (nbytes < sizeof(buffer) && !feof(input)) {
        fprintf(stderr, "error while reading %s: ", input_fname);
        perror("");
        goto end_of_execute;
    }

    res = MDI_Decoder_init(&decoder, interface, (MDI_Processor_t)0, NULL);
    if (res != 0) {
        fprintf(stderr, "error creating Decoder\n");
        goto end_of_execute;
    }

    if (verbose >= 2) {
        res = MDI_Disassembler_init(&disassembler, interface, (MDI_Processor_t)0, NULL);
        if (res != 0) {
            fprintf(stderr, "error creating Disassembler\n");
            goto end_of_execute;
        }
    }
            
    res = MDI_Execution_init(&execution, interface, (MDI_Processor_t)0, NULL);
    if (res != 0) {
        fprintf(stderr, "error creating Execution\n");
        goto end_of_execute;
    }

    next_pc = MDI_Execution_pc(execution);
    stop_pc = next_pc; /* Assume processor stopped if PC at reset is reach again. */
    fprintf(stdout, "Start of execution at PC: %"PRIuPTR"\n", next_pc);
    while (1) {
        char *current_ptr;
        MDI_res_t res;
        MDI_Operation_t operation;
        MDI_size_t pc;

        pc = next_pc;
        current_ptr = buffer + pc;
        if (verbose >= 2) {
            fprintf(stderr, "%.16s[...]\n", current_ptr);
        }
        operation = MDI_Decoder_decode(decoder, buffer, nbytes, (MDI_ptr_t *)&current_ptr);
        if (operation == NULL) {
            fprintf(stderr, "%s: invalid operation decode at PC: %"PRIuPTR"\n", input_fname, pc);
            goto end_of_execute;
        }
        if (verbose >= 2) {
            fprintf(stdout, "decoded operation at PC: %"PRIuPTR", next operation PC: %"PRIuPTR"\n",
                    pc, current_ptr - buffer);
        }
        if (verbose >= 2) {
            static char print_buffer[32];
            MDI_ptr_mut_t current = print_buffer;
            MDI_Disassembler_disassemble(disassembler, operation, print_buffer, sizeof(print_buffer), &current);
            fprintf(stdout, "  op: %s\n", print_buffer);
        }

        res = MDI_Execution_execute(execution, operation);
        if (res != 0) {
            fprintf(stderr, "%s: invalid operation execution at PC: %"PRIuPTR"\n", input_fname, pc);
            goto end_of_execute;
        }
        if (MDI_Operation_decode_info(operation)) {
            MDI_DecodeInfo_t decode_info = MDI_Operation_decode_info(operation);
            MDI_DecodeInfo_fini(&decode_info);
        }
        MDI_Operation_fini(&operation);

        count += 1;
        next_pc = MDI_Execution_pc(execution);
        if (verbose >= 2) {
            fprintf(stdout, "executed operation at PC: %"PRIuPTR", next execution PC: %"PRIuPTR"\n",
                    pc, next_pc);
        }
        if (next_pc == pc) {
            if (verbose >= 1) {
                fprintf(stdout, "processor PC busy loop, assuming stopped at PC: %"PRIuPTR"\n",
                        next_pc);
            }
            break;
        }
        if (next_pc == stop_pc) {
            if (verbose >= 1) {
                fprintf(stdout, "processor PC stop value, assuming reset at PC: %"PRIuPTR"\n",
                        next_pc);
            }
            break;
        }
    }
    if (verbose >= 1) {
        fprintf(stdout, "End of execution at PC: %"PRIuPTR"\n", MDI_Execution_pc(execution));
        fprintf(stdout, "  Insrructions count: %"PRIu64"\n", count);
        fprintf(stdout, "  Return value ret0: %"PRIu64"\n",
                MDI_Execution_ret0(execution));
    }
    rcode = 0;
 end_of_execute:
    if (decoder != NULL) MDI_Decoder_fini(&decoder);
    if (disassembler != NULL) MDI_Disassembler_fini(&disassembler);
    if (execution != NULL) MDI_Execution_fini(&execution);
    if (input != NULL && input != stdin) fclose(input);
    return rcode;
}

int main(int argc, char *argv[])
{
    char *input_fname;
    int rcode;
    MDI_interface_t interface;

    if (argc < 2) {
        fprintf(stderr, "missign argument\n");
        exit(1);
    }
    input_fname = argv[1];

    rcode = MDI_interface_init(&interface, NULL);
    if (rcode != 0) {
        fprintf(stderr, "can't initialize MDI interface\n");
        exit(1);
    }

    rcode = execute(interface, input_fname);
    if (rcode != 0) {
        fprintf(stderr, "error while executing %s\n", input_fname);
        exit(1);
    }

    rcode = MDI_interface_fini(&interface);
    if (rcode != 0) {
        fprintf(stderr, "can't destroy MDI interface\n");
        exit(1);
    }

    return 0;
}
