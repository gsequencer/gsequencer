#!/bin/sh
# 
# Copyright (C) 2005-2024 Joël Krähemann
# 
# Copying and distribution of this file, with or without modification,
# are permitted in any medium without royalty provided the copyright
# notice and this notice are preserved.  This file is offered as-is,
# without any warranty.

# ==============================================================================
# title:           fix online html
# description:     This script fixes paths to the online gtk-doc installation.
#                  The purpose is to substitute URIs.
# author:          Joël Krähemann
# date:            Fri Jan  8 21:50:20 UTC 2021
# version:         v0.1.0
# usage:           bash fix-online-html.sh
# dependencies:    find, sed,
# ==============================================================================

srcdir="./"
builddir="./"

$( find ${builddir}/docs/reference/libags/libags-html -name "*.html" -type f -exec sed -i "s@../gobject@https://docs.gtk.org/gobject/@g" {} \; )
$( find ${builddir}/docs/reference/libags-audio/libags-audio-html -name "*.html" -type f -exec sed -i "s@../gobject@https://docs.gtk.org/gobject/@g" {} \; )
$( find ${builddir}/docs/reference/libags-audio/libags-audio-html -name "*.html" -type f -exec sed -i "s@../libags-html@../libags@g" {} \; )
$( find ${builddir}/docs/reference/libags-gui/libags-gui-html -name "*.html" -type f -exec sed -i "s@../gobject@https://docs.gtk.org/gobject/@g" {} \; )
$( find ${builddir}/docs/reference/libags-gui/libags-gui-html -name "*.html" -type f -exec sed -i "s@../gtk4@https://docs.gtk.org/gtk4/@g" {} \; )
$( find ${builddir}/docs/reference/libgsequencer/libgsequencer-html -name "*.html" -type f -exec sed -i "s@../gobject@https://docs.gtk.org/gobject/@g" {} \; )
$( find ${builddir}/docs/reference/libgsequencer/libgsequencer-html -name "*.html" -type f -exec sed -i "s@../gtk4@https://docs.gtk.org/gtk4/@g" {} \; )
$( find ${builddir}/docs/reference/libgsequencer/libgsequencer-html -name "*.html" -type f -exec sed -i "s@../libags-html@../libags@g" {} \; )
$( find ${builddir}/docs/reference/libgsequencer/libgsequencer-html -name "*.html" -type f -exec sed -i "s@../libags-audio-html@../libags-audio@g" {} \; )
$( find ${builddir}/docs/reference/libgsequencer/libgsequencer-html -name "*.html" -type f -exec sed -i "s@../libags-gui-html@$../libags-gui@g" {} \; )

exit 0
