#!/usr/bin/env bash
set -eou pipefail

VERBOSE="${VERBOSE:-0}"

mdi_lib="${1?}"
input="${2--}"
output="${3--}"

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

mdi_decode_c="$TOOLS_SRCDIR/mdi-decode.c"

[ "$VERBOSE" = 0 ] || echo "Compiling mdi-decode.c"
[ "$VERBOSE" = 0 ] || echo "$CC $CFLAGS $MDI_CFLAGS -c -o mdi-decode.o \"$mdi_decode_c\""
$CC $CFLAGS $MDI_CFLAGS -c -o "$tmpdir"/mdi-decode.o "$mdi_decode_c"

[ "$VERBOSE" = 0 ] || echo "Linking mdi-decode.o with given library: \"$mdi_lib\""
[ "$VERBOSE" = 0 ] || echo "$CCLD $LDFLAGS $MDI_LDFLAGS mdi_decode.o -o mdi-decode  \"$mdi_lib\""
$CCLD $LDFLAGS $MDI_LDFLAGS "$tmpdir"/mdi-decode.o -o "$tmpdir"/mdi-decode  "$mdi_lib"

[ "$VERBOSE" = 0 ] || echo "Decoding \"$input\" to \"$output\""
[ "$VERBOSE" = 0 ] || echo "mdi-decode \"$input\" \"$output\""
"$tmpdir"/mdi-decode "$input" "$output"
