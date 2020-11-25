#!/bin/sh

gsequencer_docdir=${1:-'/usr/local/share/doc/gsequencer'}

mkdir -p "${DESTDIR}${gsequencer_docdir}/user-manual"
mkdir -p "${DESTDIR}${gsequencer_docdir}/developer-manual"
mkdir -p "${DESTDIR}${gsequencer_docdir}/osc-manual"
mkdir -p "${DESTDIR}${gsequencer_docdir}/images"
mkdir -p "${DESTDIR}${gsequencer_docdir}/images/adwaita"

install -c -p -m 644 ${MESON_BUILD_ROOT}/html/user-docs/* "${DESTDIR}${gsequencer_docdir}/user-manual/"

install -c -p -m 644 ${MESON_BUILD_ROOT}/html/developer-docs/* "${DESTDIR}${gsequencer_docdir}/developer-manual/"

install -c -p -m 644 ${MESON_BUILD_ROOT}/html/osc-docs/* "${DESTDIR}${gsequencer_docdir}/osc-manual/"
