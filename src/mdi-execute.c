
#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <MDI/mdi.h>
#include <MDI/mdi_utils.h>

static int verbose = 1;

int execute(MDI_interface_t interface, const char *input_fname)
{
    static char buffer[4096];
    int rcode = -1;
    FILE *input;
    size_t nbytes;
    MDI_Execution_t execution = NULL;
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
    MDI_Execution_init(&execution, NULL);
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
        operation = MDI_Operation_decode(interface, buffer, nbytes, (MDI_ptr_t *)&current_ptr);
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
            MDI_Operation_print(interface, operation, print_buffer, sizeof(print_buffer), &current);
            fprintf(stdout, "  op: %s\n", print_buffer);
        }

        res = MDI_Operation_execute(execution, operation);
        if (res != 0) {
            fprintf(stderr, "%s: invalid operation execution at PC: %"PRIuPTR"\n", input_fname, pc);
            goto end_of_execute;
        }
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
