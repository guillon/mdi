#!/usr/bin/env bash
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

set -eou pipefail

VERBOSE="${VERBOSE:-0}"

mdi_lib="${1?}"

cleanup() {
    local code=$?
    trap - INT TERM EXIT
    [ ! -d "${tmpdir-}" ] || rm -rf "$tmpdir"
    exit "$code"
}
trap cleanup INT TERM EXIT
tmpdir="$(mktemp -d)"

prefix="$(readlink -e "$(dirname "$0")"/..)"
MDI_PREFIX="${MDI_PREFIX:-$prefix}"
MDI_INCDIR="${MDI_INCDIR:-$MDI_PREFIX/include}"
MDI_CFLAGS="${MDI_CFLAGS:--I$MDI_INCDIR}"
MDI_LDFLAGS="${MDI_LDFLAGS:-$MDI_CFLAGS}"
TOOLS_PREFIX="${TOOLS_PREFIX:-$prefix}"
TOOLS_SRCDIR="${TOOLS_SRCDIR:-$TOOLS_PREFIX/share/mdi/src}"
MDILIBS_PREFIX="${MDILIBS_PREFIX:-$prefix}"
MDILIBS_LIBEXECDIR="${MDILIBS_LIBEXECDIR:-$MDILIBS_PREFIX/libexec/mdi}"

CC="${CC:-gcc}"
CCLD="${CCLD:-$CC}"
CFLAGS="${CFLAGS:--O2 -g -Wall}"
LDFLAGS="${LDFLAGS:-$CFLAGS}"

[ -f "$mdi_lib" ] || mdi_lib="$MDILIBS_LIBEXECDIR/$mdi_lib/libmdi.so"

mdi_validate_c="$TOOLS_SRCDIR/mdi-validate.c"

[ "$VERBOSE" = 0 ] || echo "Compiling mdi-validate.c"
[ "$VERBOSE" = 0 ] || echo "$CC $CFLAGS $MDI_CFLAGS -c -o mdi-validate.o \"$mdi_validate_c\""
$CC $CFLAGS $MDI_CFLAGS -c -o "$tmpdir"/mdi-validate.o "$mdi_validate_c"

[ "$VERBOSE" = 0 ] || echo "Linking mdi-validate.o with given library: \"$mdi_lib\""
[ "$VERBOSE" = 0 ] || echo "$CCLD $LDFLAGS $MDI_LDFLAGS mdi_validate.o -o mdi-validate  \"$mdi_lib\""
$CCLD $LDFLAGS $MDI_LDFLAGS "$tmpdir"/mdi-validate.o -o "$tmpdir"/mdi-validate  "$mdi_lib"

[ "$VERBOSE" = 0 ] || echo "Executing validation"
[ "$VERBOSE" = 0 ] || echo "${EXEC-} mdi-validate"
${EXEC-} "$tmpdir"/mdi-validate
