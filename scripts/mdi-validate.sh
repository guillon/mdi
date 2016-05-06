#!/usr/bin/env bash
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
