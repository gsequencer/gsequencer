#!/bin/bash
# 
# Copyright (C) 2005-2021 Joël Krähemann
# 
# Copying and distribution of this file, with or without modification,
# are permitted in any medium without royalty provided the copyright
# notice and this notice are preserved.  This file is offered as-is,
# without any warranty.

# ==============================================================================
# title:           update po
# description:     This script updates the po directory. The purpose is
#                  to update po/POTFILES.in.in.
# author:          Joël Krähemann
# date:            Fri Jan  8 08:02:47 UTC 2021
# version:         v0.1.0
# usage:           bash update-po.sh
# dependencies:    find, grep, sort, diff, cat, sed
# ==============================================================================

srcdir="./"
builddir="./"

if [ ! -z "$SRCDIR" ]
then
    srcdir=$SRCDIR
else
    srcdir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
fi

if [ ! -z "$BUILDDIR" ]
then
    builddir=$BUILDDIR
else
    builddir="$( pwd )"
fi

$( find ${srcdir}/ags/{util,lib,object,thread,file,server,plugin,audio,widget,X}/ -name "*.c" -print | grep -vZf ${srcdir}/pot_exclude.txt | sed "s@${srcdir}/@@g" | sort > ${builddir}/po/POTFILES.in.2 ; )

if diff ${srcdir}/po/POTFILES.in ${builddir}/po/POTFILES.in.2 > /dev/null 2>&1;
then
    rm -f ${builddir}/po/POTFILES.in.2
else
    rm ${srcdir}/po/POTFILES.in
    touch ${srcdir}/po/POTFILES.in
    rm ${srcdir}/po/POTFILES.in.in
    touch ${srcdir}/po/POTFILES.in.in
    cat ${srcdir}/license-notice-gnu-gpl-3-0+-sym.txt >> ${srcdir}/po/POTFILES.in.in
    echo -e -n "\n" >> ${srcdir}/po/POTFILES.in.in
    cat ${srcdir}/po/POTFILES.in.2 >> ${srcdir}/po/POTFILES.in.in
    mv ${srcdir}/po/POTFILES.in.2 ${srcdir}/po/POTFILES.in
fi
