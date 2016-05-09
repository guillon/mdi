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

# One of: debug, release
MODE=debug

CFLAGS_debug=-O0 -g
LDFLAGS_debug=$(CFLAGS_debug)
LIBS_debug=
CFLAGS_release=-O2 -g
LDFLAGS_release=$(CFLAGS_debug)
LIBS_release=

CC=gcc
CCLD=$(CC)
CFLAGS=$(CFLAGS_$(MODE))
LDFLAGS=$(LDFLAGS_$(MODE))
LIBS=$(LIBS_$(MODE))

PREFIX=/usr/local
BUILD_MDI=$(abspath ./build/$(MODE)/mdi)
BUILD_TOOLS=$(abspath ./build/$(MODE)/tools)
BUILD_DOC=$(abspath ./build/$(MODE)/doc)
BUILD_EXAMPLES=$(abspath ./build/$(MODE)/examples)

all: all-mdi all-tools all-examples all-doc

check: check-examples

clean: clean-mdi clean-tools clean-examples clean-doc

install: install-mdi install-tools install-examples install-doc

distclean: clean

all-mdi:
	mkdir -p $(BUILD_MDI)/include/MDI
	cp -a include/MDI/mdi.h $(BUILD_MDI)/include/MDI/
	cp -a include/MDI/mdi_operations.h $(BUILD_MDI)/include/MDI/

clean-mdi:
	rm -rf $(BUILD_MDI)

install-mdi: all-mdi
	mkdir -p $(PREFIX)
	cp -aT $(BUILD_MDI) $(PREFIX)

all-tools:
	mkdir -p $(BUILD_TOOLS)/share/mdi/src
	cp -a src/mdi-validate.c $(BUILD_TOOLS)/share/mdi/src/
	cp -a src/mdi-execute.c $(BUILD_TOOLS)/share/mdi/src/
	cp -a src/mdi-decode.c $(BUILD_TOOLS)/share/mdi/src/
	mkdir -p $(BUILD_TOOLS)/bin
	cp -Ta scripts/mdi-validate.sh $(BUILD_TOOLS)/bin/mdi-validate
	cp -Ta scripts/mdi-execute.sh $(BUILD_TOOLS)/bin/mdi-execute
	cp -Ta scripts/mdi-decode.sh $(BUILD_TOOLS)/bin/mdi-decode

clean-tools:
	rm -rf $(BUILD_TOOLS)

install-tools: all-tools
	mkdir -p $(PREFIX)
	cp -aT $(BUILD_TOOLS) $(PREFIX)

all-examples: all-mdi
	make -C examples/mini -f $(abspath examples/mini/GNUmakefile) \
	  PREFIX="$(PREFIX)" \
	  BUILD="$(BUILD_EXAMPLES)" \
	  MDI_PREFIX="$(BUILD_MDI)" \
	  all

clean-examples:
	make -C examples/mini -f $(abspath examples/mini/GNUmakefile) \
	  PREFIX="$(PREFIX)" \
	  BUILD="$(BUILD_EXAMPLES)" \
	  MDI_PREFIX="$(BUILD_MDI)" \
	  clean

install-examples:all-examples
	make -C examples/mini -f $(abspath examples/mini/GNUmakefile) \
	  PREFIX="$(PREFIX)" \
	  BUILD="$(BUILD_EXAMPLES)" \
	  MDI_PREFIX="$(BUILD_MDI)" \
	  install

check-examples: all-examples all-tools
	make -C examples/mini -f $(abspath examples/mini/GNUmakefile) \
	  PREFIX="$(PREFIX)" \
	  BUILD="$(BUILD_EXAMPLES)" \
	  MDI_PREFIX="$(BUILD_MDI)" \
	  TOOLS_PREFIX="$(BUILD_TOOLS)" \
	  check

all-doc:
	mkdir -p $(BUILD_DOC)/share/mdi/doc
	cp doc/MDI/Doxyfile $(BUILD_DOC)/share/mdi/doc
	echo 'OUTPUT_DIRECTORY = "$(BUILD_DOC)/share/mdi/doc"' >> $(BUILD_DOC)/share/mdi/doc/Doxyfile
	doxygen $(BUILD_DOC)/share/mdi/doc/Doxyfile

clean-doc:
	rm -rf $(BUILD_DOC)

install-doc: all-doc
	mkdir -p $(PREFIX)
	cp -aT $(BUILD_DOC) $(PREFIX)


distclean: clean
