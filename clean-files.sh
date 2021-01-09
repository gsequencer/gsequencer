#!/bin/bash
# 
# Copyright (C) 2005-2021 Joël Krähemann
# 
# Copying and distribution of this file, with or without modification,
# are permitted in any medium without royalty provided the copyright
# notice and this notice are preserved.  This file is offered as-is,
# without any warranty.

# ==============================================================================
# title:           clean build
# description:     This script cleans the build directory. The purpose is to
#                  remove files.
# author:          Joël Krähemann
# date:            Fri Jan  8 08:02:47 UTC 2021
# version:         v0.1.0
# usage:           bash clean.sh
# ==============================================================================

rm -f gsequencer.desktop
rm -f libags.sym
rm -f libags_audio.sym
rm -f libags_gui.sym
rm -f libags_server.sym
rm -f libags_thread.sym
rm -f changelog.gz

rm -f ags/object/ags_marshallers.list
rm -f ags/widget/ags_widget_marshallers.list

rm -f docs/listings/add_pattern.c
rm -f docs/listings/application_mutex.c
rm -f docs/listings/audio.c
rm -f docs/listings/audio_application_context.c
rm -f docs/listings/complete_example.c
rm -f docs/listings/config.c
rm -f docs/listings/effects_echo_audio.c
rm -f docs/listings/effects_echo_audio_run.c
rm -f docs/listings/effects_echo_channel.c
rm -f docs/listings/effects_echo_channel_run.c
rm -f docs/listings/effects_recall_container.c
rm -f docs/listings/file_property.c
rm -f docs/listings/file_read.c
rm -f docs/listings/file_write.c
rm -f docs/listings/find_port.c
rm -f docs/listings/fx_factory.c
rm -f docs/listings/linking_prerequisites.c
rm -f docs/listings/linking_safe.c
rm -f docs/listings/linking_unsafe.c
rm -f docs/listings/notation_clipboard.c
rm -f docs/listings/pcm_info.c
rm -f docs/listings/port.c
rm -f docs/listings/pull_thread.c
rm -f docs/listings/recycling.c
rm -f docs/listings/start_thread.c
rm -f docs/listings/thread_application_context.c
rm -f docs/listings/thread_obj_mutex.c

rm -rf docs/developersBook/xml/

rm -f docs/reference/libags-audio/libags_audio-sections.txt
rm -f docs/reference/libags-audio/libags_audio.interfaces
rm -f docs/reference/libags-audio/libags_audio.types
rm -f docs/reference/libags-gui/libags_gui-sections.txt
rm -f docs/reference/libags-gui/libags_gui.interfaces
rm -f docs/reference/libags-gui/libags_gui.types
rm -f docs/reference/libags/libags-sections.txt
rm -f docs/reference/libags/libags.interfaces
rm -f docs/reference/libags/libags.types
rm -f docs/reference/libgsequencer/libgsequencer-sections.txt
rm -f docs/reference/libgsequencer/libgsequencer.interfaces
rm -f docs/reference/libgsequencer/libgsequencer.types

exit 0
