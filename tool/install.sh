#!/bin/sh

die() {
    printf '%s\n' "$1"; exit 1
}

[ -z "$1" ]      && die "usage: $0 [destination]"
[ -f "chmap" ]   || die "can't find chmap"
[ -f "chmap.1" ] || die "can't find chmap's db"

install -Dm755 chmap    "$1/bin/chmap"
install -Dm644 chmap.1  "$1/share/man/man1/chmap.1"
