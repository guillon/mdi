#!/usr/bin/env python
#
# Machine Description Interface C API
#
# This software is delivered under the terms of the MIT License
#
# Copyright (c) 2016 STMicroelectronics
#
# Permission is hereby granted, free of charge, to any person
# obtaining a copy of this software and associated documentation
# files (the "Software"), to deal in the Software without
# restriction, including without limitation the rights to use,
# copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following
# conditions:
#
# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
# OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
# HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
# WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
# OTHER DEALINGS IN THE SOFTWARE.
#
#

from __future__ import print_function
import sys

class ENUM:

    instructions_list = []
    def __init__(self, ID, mnemonic, properties, parsing, encoding, short_desc, execution, description):
        self.ID = ID
        self.mnemonic = mnemonic
        self.properties = properties
        self.parsing = parsing
        self.encoding = encoding
        self.short_desc = short_desc
        self.execution = execution
        self.description = description
        self.instructions_list.append(self)
        
    @staticmethod
    def emit_execution(out):
        with open(out, "w") as outf:
            print("/* BEGIN: Generated executions */", file=outf)
            ENUM._emit_executions(outf)
            print("/* END: Generated executions */", file=outf)

    @staticmethod
    def _emit_executions(out):
        idx = 0;
        print("#define P(idx) EXE_OPS(_operands,idx)", file=out)
        print("#define NEXT_PC() (RR(PC,0) + _op_size)", file=out)
        print("#define RR(rf,idx) EXE_CPU_RR((*_cpu_prev),rf,idx)", file=out)
        print("#define RS(rf,idx) EXE_CPU_RS(_cpu,rf,idx)", file=out)
        print("#define MR32(idx) EXE_MEM_FETCH32(_mem,idx)", file=out)
        print("#define MS32(idx) EXE_MEM_SLICE32(_mem,idx)", file=out)
        for inst in ENUM.instructions_list:
            print("", file=out)
            print("static int32_t _execution_%i /* %s */ (EXE_CTX_T _context, EXE_OPS_T _operands, size_t _op_size)" %
                  (idx, inst.ID), file=out)
            print("{", file=out)
            print("  CPU_T _cpu, *_cpu_prev = EXE_CTX_CPU(_context);", file=out)
            print("  MEM_T _mem, *_mem_prev = EXE_CTX_MEM(_context);", file=out)
            print("  EXE_CPU_CLONE(_cpu, _cpu_prev);", file=out)
            print("  EXE_MEM_CLONE(_mem, _mem_prev);", file=out)
            print("  RS(PC,0) = NEXT_PC();", file=out)
            print("  %s;" % inst.execution, file=out)
            print("  EXE_CPU_UPDATE(*_cpu_prev, &_cpu);", file=out);
            print("  EXE_CPU_UPDATE(*_mem_prev, &_mem);", file=out);
            print("  return 0;", file=out)
            print("}", file=out);
            idx += 1
        print("#undef RF", file=out)
        print("#undef MEM", file=out)
        print("typedef int32_t (*EXE_FUNC_T)(EXE_CTX_T _context, EXE_OPS_T _operands, size_t _op_size);", file=out);
        print("static const EXE_FUNC_T _executions[] = {", file=out)
        idx = 0
        for inst in ENUM.instructions_list:
            print("  _execution_%i /* %s */," % (idx, inst.ID), file=out)
            idx += 1
        print("};", file=out)

execfile(sys.argv[1])
ENUM.emit_execution(sys.argv[2])
