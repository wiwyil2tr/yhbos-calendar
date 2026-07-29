#!/bin/sh
set -e
pkg="$1"
tmpdir=$(mktemp -d)
dpkg-deb -R "$pkg" "$tmpdir"
ctl="$tmpdir/DEBIAN/control"
# insert multi-language descriptions before the trailing blank line
sed -i '$i\
Description-zh.UTF-8: 轻量级桌面万年历\
Description-ru.UTF-8: Легкий настольный календарь с лунным календарем' "$ctl"
dpkg-deb -b "$tmpdir" "$pkg"
rm -rf "$tmpdir"
