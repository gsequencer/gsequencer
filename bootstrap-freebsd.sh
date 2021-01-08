#!/bin/bash
# 
# Copyright (C) 2005-2021 Joël Krähemann
# 
# Copying and distribution of this file, with or without modification,
# are permitted in any medium without royalty provided the copyright
# notice and this notice are preserved.  This file is offered as-is,
# without any warranty.

# ==============================================================================
# title:           bootstrap FreeBSD build
# description:     This script bootstraps the build directory. The purpose is
#                  to copy, strip or substitute files.
#                  to generated files.
# author:          Joël Krähemann
# date:            Fri Jan  8 08:02:47 UTC 2021
# version:         v0.1.0
# usage:           bash bootstrap-freebsd.sh
# ==============================================================================

prefix="/usr/local"
datadir="${prefix}/share"

srcdir="./"

if [ ! -z "$SRCDIR" ]
then
    srcdir=$SRCDIR
else
    srcdir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
fi

while [[ "$#" -gt 0 ]]; do
    case $1 in
        --prefix) prefix="$2"; shift ;;
        --datadir) datadir="$2"; shift ;;
        *) echo "Unknown parameter passed: $1"; exit 1 ;;
    esac
    shift
done

# generate symbols files
echo "generating libags.sym"
tail -n +19 $srcdir/libags.sym.in > libags.sym

echo "generating libags_thread.sym"
tail -n +19 $srcdir/libags_thread.sym.in > libags_thread.sym

echo "generating libags_server.sym"
tail -n +19 $srcdir/libags_server.sym.in > libags_server.sym

echo "generating libags_audio.sym"
tail -n +19 $srcdir/libags_audio.sym.in > libags_audio.sym

echo "generating libags_gui.sym"
tail -n +19 $srcdir/libags_gui.sym.in > libags_gui.sym

# generate marshallers list
echo "generating marshallers list"

mkdir -p ags/object/
tail -n +19 $srcdir/ags/object/ags_marshallers.list.in > ags/object/ags_marshallers.list

mkdir -p ags/widget/
tail -n +19 $srcdir/ags/widget/ags_widget_marshallers.list.in > ags/widget/ags_widget_marshallers.list

# generate marshallers list
echo "generating po/POTFILES.in"

mkdir -p po
tail -n +19 $srcdir/po/POTFILES.in.in > po/POTFILES.in

# copy documentation listings
echo "generating resources for API Reference Manual"

mkdir -p docs/reference/libags
tail -n +19 $srcdir/docs/reference/libags/libags-sections.txt.in > docs/reference/libags/libags-sections.txt
tail -n +19 $srcdir/docs/reference/libags/libags.types.in > docs/reference/libags/libags.types
tail -n +19 $srcdir/docs/reference/libags/libags.interfaces.in > docs/reference/libags/libags.interfaces

mkdir -p docs/reference/libags-audio
tail -n +19 $srcdir/docs/reference/libags-audio/libags_audio-sections.txt.in > docs/reference/libags-audio/libags_audio-sections.txt
tail -n +19 $srcdir/docs/reference/libags-audio/libags_audio.types.in > docs/reference/libags-audio/libags_audio.types
tail -n +19 $srcdir/docs/reference/libags-audio/libags_audio.interfaces.in > docs/reference/libags-audio/libags_audio.interfaces

mkdir -p docs/reference/libags-gui
tail -n +19 $srcdir/docs/reference/libags-gui/libags_gui-sections.txt.in > docs/reference/libags-gui/libags_gui-sections.txt
tail -n +19 $srcdir/docs/reference/libags-gui/libags_gui.types.in > docs/reference/libags-gui/libags_gui.types
tail -n +19 $srcdir/docs/reference/libags-gui/libags_gui.interfaces.in > docs/reference/libags-gui/libags_gui.interfaces

mkdir -p docs/reference/libgsequencer
tail -n +19 $srcdir/docs/reference/libgsequencer/libgsequencer-sections.txt.in > docs/reference/libgsequencer/libgsequencer-sections.txt
tail -n +19 $srcdir/docs/reference/libgsequencer/libgsequencer.types.in > docs/reference/libgsequencer/libgsequencer.types
tail -n +19 $srcdir/docs/reference/libgsequencer/libgsequencer.interfaces.in > docs/reference/libgsequencer/libgsequencer.interfaces

# copy documentation listings
echo "generating code listings"

mkdir -p docs/listings/

tail -n +10 $srcdir/docs/listings/add_pattern.c.in > docs/listings/add_pattern.c
tail -n +10 $srcdir/docs/listings/application_mutex.c.in > docs/listings/application_mutex.c
tail -n +10 $srcdir/docs/listings/audio.c.in > docs/listings/audio.c
tail -n +10 $srcdir/docs/listings/audio_application_context.c.in > docs/listings/audio_application_context.c
tail -n +10 $srcdir/docs/listings/complete_example.c.in > docs/listings/complete_example.c
tail -n +10 $srcdir/docs/listings/config.c.in > docs/listings/config.c
tail -n +10 $srcdir/docs/listings/effects_echo_audio.c.in > docs/listings/effects_echo_audio.c
tail -n +10 $srcdir/docs/listings/effects_echo_audio_run.c.in > docs/listings/effects_echo_audio_run.c
tail -n +10 $srcdir/docs/listings/effects_echo_channel.c.in > docs/listings/effects_echo_channel.c
tail -n +10 $srcdir/docs/listings/effects_echo_channel_run.c.in > docs/listings/effects_echo_channel_run.c
tail -n +10 $srcdir/docs/listings/effects_recall_container.c.in > docs/listings/effects_recall_container.c
tail -n +10 $srcdir/docs/listings/file_property.c.in > docs/listings/file_property.c
tail -n +10 $srcdir/docs/listings/file_read.c.in > docs/listings/file_read.c
tail -n +10 $srcdir/docs/listings/file_write.c.in > docs/listings/file_write.c
tail -n +10 $srcdir/docs/listings/find_port.c.in > docs/listings/find_port.c
tail -n +10 $srcdir/docs/listings/fx_factory.c.in > docs/listings/fx_factory.c
tail -n +10 $srcdir/docs/listings/linking_prerequisites.c.in > docs/listings/linking_prerequisites.c
tail -n +10 $srcdir/docs/listings/linking_safe.c.in > docs/listings/linking_safe.c
tail -n +10 $srcdir/docs/listings/linking_unsafe.c.in > docs/listings/linking_unsafe.c
tail -n +10 $srcdir/docs/listings/notation_clipboard.c.in > docs/listings/notation_clipboard.c
tail -n +10 $srcdir/docs/listings/pcm_info.c.in > docs/listings/pcm_info.c
tail -n +10 $srcdir/docs/listings/port.c.in > docs/listings/port.c
tail -n +10 $srcdir/docs/listings/pull_thread.c.in > docs/listings/pull_thread.c
tail -n +10 $srcdir/docs/listings/recycling.c.in > docs/listings/recycling.c
tail -n +10 $srcdir/docs/listings/start_thread.c.in > docs/listings/start_thread.c
tail -n +10 $srcdir/docs/listings/thread_application_context.c.in > docs/listings/thread_application_context.c
tail -n +10 $srcdir/docs/listings/thread_obj_mutex.c.in > docs/listings/thread_obj_mutex.c

# generate gsequencer.desktop
echo "generating gsequencer.desktop"

$(sed -e "s,\@datadir\@,${datadir},g" < "${srcdir}/gsequencer.desktop.in" > "gsequencer.desktop")
