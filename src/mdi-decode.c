
#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <MDI/mdi.h>
#include <MDI/mdi_utils.h>

static int verbose = 2;

int print(MDI_interface_t interface, MDI_Operation_t *list, size_t list_size, const char *output_fname)
{
    int rcode = -1;
    FILE *output;
    static char buffer[4096];
    char *current;
    int i;

    if (strcmp(output_fname, "-") == 0) {
        output = stdout;
    } else {
        output = fopen(output_fname, "w");
        if (output == NULL) {
            fprintf(stderr, "error opening %s: ", output_fname);
            perror("");
            goto end_of_print;
        }
    }

    if (verbose >= 1) {
        fprintf(stderr, "Start of Disassembly\n");
    }

    current = buffer;
    for (i = 0; i < list_size; i++) {
        char *next = current;
        int nbytes = MDI_Operation_print(interface, list[i], buffer, sizeof(buffer), &next);
        if (current + nbytes >= buffer + sizeof(buffer)) {
            fprintf(stderr, "not enough space to print");
            goto end_of_print;
        }
        fprintf(output, "  %s\n", current);
        current = next;
    }
    if (verbose >= 1) {
        fprintf(stdout, "End of Disassembly.\n");
    }
    rcode = 0;
 end_of_print:
    if (output != NULL && output != stdout) fclose(output);
    return rcode;
}

int decode(MDI_interface_t interface, MDI_Operation_t **list, size_t *list_size, const char *input_fname)
{
    static char buffer[4096];
    int rcode = -1;
    FILE *input;
    char *current_ptr;
    size_t nbytes;
    size_t current_offset = 0;
    MDI_Operation_t operation;
    MDI_Operation_t *oplist = NULL;
    size_t opcount = 0;

    if (strcmp(input_fname, "-") == 0) {
        input = stdin;
    } else {
        input = fopen(input_fname, "rb");
        if (input == NULL) {
            fprintf(stderr, "error opening %s: ", input_fname);
            perror("");
            goto end_of_decode;
        }
    }

    nbytes = fread(buffer, 1, sizeof(buffer), input);
    if (nbytes < sizeof(buffer) && !feof(input)) {
        fprintf(stderr, "error while reading %s: ", input_fname);
        perror("");
        goto end_of_decode;
    }

    if (verbose >= 1) {
        fprintf(stderr, "Start of Decode\n");
    }
    
    while(current_offset < nbytes) {
        current_ptr = buffer + current_offset;
        operation = MDI_Operation_decode(interface, buffer, nbytes, (MDI_ptr_t *)&current_ptr);
        if (operation == NULL) {
            fprintf(stderr, "%s: invalid operation at offset: %"PRIiPTR"\n", input_fname, current_offset);
            goto end_of_decode;
        }
        if (verbose >= 2) {
            fprintf(stderr, "  decoded operation at offset: %"PRIiPTR", next: %"PRIiPTR"\n",
                    current_offset, current_ptr - buffer);
        }
        current_offset = current_ptr - buffer;
        oplist = (MDI_Operation_t *)realloc(oplist, sizeof(MDI_Operation_t) * (opcount + 1));
        oplist[opcount] = operation;
        opcount++;
    }

    *list = oplist;
    *list_size = opcount;
    if (verbose >= 1) {
        fprintf(stdout, "End of Decode. (%"PRIuPTR" operations, %"PRIuPTR" bytes)\n",
                opcount, current_offset);
    }
    rcode = 0;
 end_of_decode:
    if (input != NULL && input != stdin) fclose(input);
    return rcode;
}

int main(int argc, char *argv[])
{
    char *input_fname, *output_fname;
    int rcode;
    MDI_interface_t interface;
    MDI_Operation_t *list;
    size_t count;

    if (argc < 3) {
        fprintf(stderr, "missign argument\n");
        exit(1);
    }
    input_fname = argv[1];
    output_fname = argv[2];

    rcode = MDI_interface_init(&interface, NULL);
    if (rcode != 0) {
        fprintf(stderr, "can't initialize MDI interface\n");
        exit(1);
    }

    rcode = decode(interface, &list, &count, input_fname);
    if (rcode != 0) {
        fprintf(stderr, "error while decoding from %s\n", input_fname);
        exit(1);
    }

    rcode = print(interface, list, count, output_fname);
    if (rcode != 0) {
        fprintf(stderr, "error while printing to %s\n", output_fname);
        exit(1);
    }

    rcode = MDI_interface_fini(&interface);
    if (rcode != 0) {
        fprintf(stderr, "can't destroy MDI interface\n");
        exit(1);
    }

    return 0;
}
