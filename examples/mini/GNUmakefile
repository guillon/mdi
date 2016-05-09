#
# Machine Description Interface Implementation for MINI platform.
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

MODE=debug

CFLAGS_debug=-O0 -g
LDFLAGS_debug=$(CFLAGS_debug)
LIBS_debug=
CFLAGS_release=-O2 -g
LDFLAGS_release=$(CFLAGS_debug)
LIBS_release=

PYTHON=python
CC=gcc
CCLD=$(CC)
CFLAGS=$(CFLAGS_$(MODE))
LDFLAGS=$(LDFLAGS_$(MODE))
LIBS=$(LIBS_$(MODE))

PREFIX=/usr/local
BUILD=./build/$(MODE)

CC=gcc

ALL_CFLAGS=$(CFLAGS) -Wall -fPIC $(MDI_CFLAGS) -I.
ALL_LDFLAGS=$(LDFLAGS) $(MDI_LDFLAGS)
ALL_LIBS=$(LIBS) $(MDI_LIBS)

MDI_PREFIX=$(PREFIX)
MDI_CFLAGS=-I $(MDI_PREFIX)/include
MDI_LDFLAGS=
MDI_LIBS=

TOOLS_PREFIX=$(PREFIX)

ENUMS=mde/instructions.enum mde/platform.enum
OBJS=mdi.o mdi_operation.o mdi_execution.o mdi_disassembler.o mdi_decoder.o
LIB_A=libmdi.a
LIB_SO=libmdi.so

all: $(LIB_A) $(LIB_SO)
	mkdir -p $(BUILD)/libexec/mdi/mini
	cp -a $(LIB_A) $(LIB_SO) $(BUILD)/libexec/mdi/mini/
	mkdir -p $(BUILD)/share/mdi/mini/tests
	cp -a tests/mini_trap.enc $(BUILD)/share/mdi/mini/tests
	cp -a tests/mini_loop.enc $(BUILD)/share/mdi/mini/tests

install: all
	mkdir -p $(PREFIX)
	cp -aT $(BUILD) $(PREFIX)

clean:
	rm -f $(LIB_A) $(LIB_SO) $(OBJS)
	rm -f generated*.inc
	rm -rf $(BUILD)

distclean: clean

check:
	env TOOLS_PREFIX="$(TOOLS_PREFIX)" MDI_PREFIX="$(MDI_PREFIX)" MDILIBS_PREFIX="$(BUILD)" $(TOOLS_PREFIX)/bin/mdi-validate mini
	env TOOLS_PREFIX="$(TOOLS_PREFIX)" MDI_PREFIX="$(MDI_PREFIX)" MDILIBS_PREFIX="$(BUILD)" $(TOOLS_PREFIX)/bin/mdi-decode mini $(BUILD)/share/mdi/mini/tests/mini_loop.enc
	env TOOLS_PREFIX="$(TOOLS_PREFIX)" MDI_PREFIX="$(MDI_PREFIX)" MDILIBS_PREFIX="$(BUILD)" $(TOOLS_PREFIX)/bin/mdi-decode mini $(BUILD)/share/mdi/mini/tests/mini_trap.enc
	env TOOLS_PREFIX="$(TOOLS_PREFIX)" MDI_PREFIX="$(MDI_PREFIX)" MDILIBS_PREFIX="$(BUILD)" $(TOOLS_PREFIX)/bin/mdi-execute mini $(BUILD)/share/mdi/mini/tests/mini_loop.enc
	env TOOLS_PREFIX="$(TOOLS_PREFIX)" MDI_PREFIX="$(MDI_PREFIX)" MDILIBS_PREFIX="$(BUILD)" $(TOOLS_PREFIX)/bin/mdi-execute mini $(BUILD)/share/mdi/mini/tests/mini_trap.enc

$(LIB_A): $(OBJS)
	ar crv $@ $^
	ranlib $@

$(LIB_SO): $(OBJS)
	$(CCLD) $(ALL_LDFLAGS) -shared $^ -o $@ $(ALL_LIBS)

$(OBJS): $(ENUMS)
$(OBJS): %.o: src/%.c
	$(CC) $(ALL_CFLAGS) -c $< -o $@

mdi_execution.o: generated_executions.inc
generated_executions.inc: mde/instructions.enum scripts/generate_executions.py
	$(PYTHON) scripts/generate_executions.py mde/instructions.enum generated_executions.inc
