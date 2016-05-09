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
#include <MDI/mdi.h>


static int failed = 0;
static int max_failed = 10;
static FILE *test_log = NULL;

static void TEST(int expr, const char *desc)
{
    if (expr) {
        fprintf(test_log, "PASSED: %s\n", desc);
    } else {
        fprintf(test_log, "FAILED: %s\n", desc);
        exit(1);
    }
}

static void CHECK(int expr, const char *desc)
{
    if (expr) {
        (void)0; /* CHECK is silent when passing */
    } else {
        if (failed >= max_failed) {
            if (failed == max_failed) {
                fprintf(test_log, "stop emmiting FAILED after %d messages\n", failed);
            }
        } else {
            fprintf(test_log, "FAILED: %s\n", desc);
        }
        failed++;
    }
}

static void CHECKPOINT(const char *desc)
{
    if (failed == 0) {
        fprintf(test_log, "PASSED: %s\n", desc);
    } else {
        fprintf(test_log, "FAILED: %d failures observed: %s\n", failed, desc);
        exit(1);
    }
    failed = 0;
}

static void validate_platform(MDI_interface_t interface)
{
    MDI_Platform_t platform;

    platform = MDI_Platform(interface);
    CHECK(MDI_Platform_ID(platform) != NULL, "Platform ID defined");
    CHECK(MDI_Platform_processors(platform) != NULL, "Platform processors defined");

    CHECKPOINT("Platform attributes validity");
}

static void validate_instructions(MDI_interface_t interface)
{
    MDI_Instruction_t instruction;
    MDI_idx_t count;
    int i;

    count = MDI_Instructions_count(interface);

    for (i = 0; i < count; i++) {
        instruction = MDI_Instructions_iter(interface, i);
        CHECK(MDI_Instruction_idx(instruction) == i, "Instruction idx mismatches");
        CHECK(MDI_Instruction_ID(instruction) != NULL, "Instruction ID defined");
        CHECK(MDI_Instruction_mnemonic(instruction) != NULL, "Instruction mnemonic defined");
        CHECK(MDI_Instruction_properties(instruction) != NULL, "Instruction properties defined");
    }
    CHECKPOINT("Instructions attributes validity");
}

static void validate_opcodes(MDI_interface_t interface)
{
    MDI_Opcode_t opcode;
    MDI_idx_t count;
    int i;

    count = MDI_Opcodes_count(interface);

    for (i = 0; i < count; i++) {
        opcode = MDI_Opcodes_iter(interface, i);
        CHECK(MDI_Opcode_idx(opcode) == i, "Opcode idx mismatches");
        CHECK(MDI_Opcode_ID(opcode) != NULL, "Opcode ID defined");
        CHECK(MDI_Instruction_idx(MDI_Opcode_instruction(opcode)) < MDI_Instructions_count(interface),
              "Opcode instruction valid");
    }
    CHECKPOINT("Opcodes attributes validity");
}

static void validate_operators(MDI_interface_t interface)
{
    MDI_Operator_t operator;
    MDI_idx_t count;
    int i;

    count = MDI_Operators_count(interface);

    for (i = 0; i < count; i++) {
        operator = MDI_Operators_iter(interface, i);
        CHECK(MDI_Operator_idx(operator) == i, "Operator idx mismatches");
        CHECK(MDI_Operator_ID(operator) != NULL, "Operator ID defined");
        CHECK(MDI_Opcode_idx(MDI_Operator_opcode(operator, NULL)) < MDI_Opcodes_count(interface),
              "Operator opcode valid");
        CHECK(MDI_Operator_attributes(operator) != NULL, "Operator properties defined");
    }
    CHECKPOINT("Operators attributes validity");
}

static void validate_interface(void)
{
    MDI_rev_t rev;
    MDI_res_t res;
    MDI_interface_t interface;

    res = MDI_interface_init(&interface, NULL);
    TEST(res == 0, "Interface creation");

    rev = MDI_interface_revision(interface);
    TEST(rev > 0, "Interface revision is not null");
    TEST(MDI_VERSION_REV_MAJOR(rev) == MDI_VERSION_MAJOR, "Interface major version match");
    TEST(rev >= MDI_VERSION_REV, "Interface minor,patch version backward compatibilitu");

    validate_platform(interface);

    validate_instructions(interface);

    validate_opcodes(interface);

    validate_operators(interface);

    res = MDI_interface_fini(&interface);
    TEST(res == 0, "Interface destruction");
}

int main(int argc, char *argv[])
{
    test_log = stdout;
    validate_interface();
    fprintf(test_log, "SUCCESS: all tests passed\n");
    return 0;
}
