#!/bin/sh
# 
# Copyright (C) 2005-2021 Joël Krähemann
# 
# Copying and distribution of this file, with or without modification,
# are permitted in any medium without royalty provided the copyright
# notice and this notice are preserved.  This file is offered as-is,
# without any warranty.

# ==============================================================================
# title:           fix local html
# description:     This script fixes paths to the local gtk-doc installation.
#                  The purpose is to substitute URIs.
# author:          Joël Krähemann
# date:            Fri Jan  8 21:50:20 UTC 2021
# version:         v0.1.0
# usage:           bash fix-html.sh
# dependencies:    find, sed,
# ==============================================================================

srcdir="./"
builddir="./"

gobject_api_doc="/usr/share/doc/libglib2.0-doc/gobject"
gtk_api_doc="/usr/share/doc/libgtk-3-doc/gtk3"
libags_api_doc="/usr/share/doc/libags-doc/api"
libags_audio_api_doc="/usr/share/doc/libags-audio-doc/api"
libags_gui_api_doc="/usr/share/doc/libags-gui-doc/api"
libgsequencer_api_doc="/usr/share/doc/libgsequencer-doc/api"

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

if [ ! -z "$GOBJECT_API_DOC" ]
then
    gobject_api_doc=$GOBJECT_API_DOC
fi

if [ ! -z "$GTK_API_DOC" ]
then
    gtk_api_doc=$GTK_API_DOC
fi

if [ ! -z "$LIBAGS_API_DOC" ]
then
    libags_api_doc=$LIBAGS_API_DOC
fi

if [ ! -z "$LIBAGS_AUDIO_API_DOC" ]
then
    libags_audio_api_doc=$LIBAGS_AUDIO_API_DOC
fi

if [ ! -z "$LIBAGS_GUI_API_DOC" ]
then
    libags_gui_api_doc=$LIBAGS_GUI_API_DOC
fi

if [ ! -z "$LIBGSEQUENCER_API_DOC" ]
then
    libgsequencer_api_doc=$LIBGSEQUENCER_API_DOC
fi

$( find ${builddir}/docs/reference/libags/libags-html -name "*.html" -type f -exec sed -i "s@../gobject@${gobject_api_doc}@g" {} \; )
$( find ${builddir}/docs/reference/libags-audio/libags-audio-html -name "*.html" -type f -exec sed -i "s@../gobject@${gobject_api_doc}@g" {} \; )
$( find ${builddir}/docs/reference/libags-audio/libags-audio-html -name "*.html" -type f -exec sed -i "s@../libags-html@${libags_api_doc}@g" {} \; )
$( find ${builddir}/docs/reference/libags-gui/libags-gui-html -name "*.html" -type f -exec sed -i "s@../gobject@${gobject_api_doc}@g" {} \; )
$( find ${builddir}/docs/reference/libags-gui/libags-gui-html -name "*.html" -type f -exec sed -i "s@../gtk3@${gtk_api_doc}@g" {} \; )
$( find ${builddir}/docs/reference/libgsequencer/libgsequencer-html -name "*.html" -type f -exec sed -i "s@../gobject@${gobject_api_doc}@g" {} \; )
$( find ${builddir}/docs/reference/libgsequencer/libgsequencer-html -name "*.html" -type f -exec sed -i "s@../gtk3@${gtk_api_doc}@g" {} \; )
$( find ${builddir}/docs/reference/libgsequencer/libgsequencer-html -name "*.html" -type f -exec sed -i "s@../libags-html@${libags_api_doc}@g" {} \; )
$( find ${builddir}/docs/reference/libgsequencer/libgsequencer-html -name "*.html" -type f -exec sed -i "s@../libags-audio-html@${libags_audio_api_doc}@g" {} \; )
$( find ${builddir}/docs/reference/libgsequencer/libgsequencer-html -name "*.html" -type f -exec sed -i "s@../libags-gui-html@${libags_gui_api_doc}@g" {} \; )
