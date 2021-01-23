# -*- mode: autoconf -*-
#
# GSequencer - Advanced GTK Sequencer 
# Copyright (C) 2005-2021 Joël Krähemann
# 
# This file is part of GSequencer.
#
# GSequencer is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# GSequencer is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GSequencer.  If not, see <http://www.gnu.org/licenses/>.

AC_DEFUN([AGS_DEVELOPER_BOOK_LISTINGS],
[
  AC_MSG_NOTICE([generating docs/listings/*])
  `mkdir -p $PWD/docs/listings/`
  `tail -n +10 $srcdir/docs/listings/add_pattern.c.in > $PWD/docs/listings/add_pattern.c`
  `tail -n +10 $srcdir/docs/listings/application_mutex.c.in > $PWD/docs/listings/application_mutex.c`
  `tail -n +10 $srcdir/docs/listings/audio.c.in > $PWD/docs/listings/audio.c`
  `tail -n +10 $srcdir/docs/listings/audio_application_context.c.in > $PWD/docs/listings/audio_application_context.c`
  `tail -n +10 $srcdir/docs/listings/complete_example.c.in > $PWD/docs/listings/complete_example.c`
  `tail -n +10 $srcdir/docs/listings/config.c.in > $PWD/docs/listings/config.c`
  `tail -n +10 $srcdir/docs/listings/effects_echo_audio.c.in > $PWD/docs/listings/effects_echo_audio.c`
  `tail -n +10 $srcdir/docs/listings/effects_echo_audio_run.c.in > $PWD/docs/listings/effects_echo_audio_run.c`
  `tail -n +10 $srcdir/docs/listings/effects_echo_channel.c.in > $PWD/docs/listings/effects_echo_channel.c`
  `tail -n +10 $srcdir/docs/listings/effects_echo_channel_run.c.in > $PWD/docs/listings/effects_echo_channel_run.c`
  `tail -n +10 $srcdir/docs/listings/effects_recall_container.c.in > $PWD/docs/listings/effects_recall_container.c`
  `tail -n +10 $srcdir/docs/listings/file_property.c.in > $PWD/docs/listings/file_property.c`
  `tail -n +10 $srcdir/docs/listings/file_read.c.in > $PWD/docs/listings/file_read.c`
  `tail -n +10 $srcdir/docs/listings/file_write.c.in > $PWD/docs/listings/file_write.c`
  `tail -n +10 $srcdir/docs/listings/find_port.c.in > $PWD/docs/listings/find_port.c`
  `tail -n +10 $srcdir/docs/listings/fx_factory.c.in > $PWD/docs/listings/fx_factory.c`
  `tail -n +10 $srcdir/docs/listings/linking_prerequisites.c.in > $PWD/docs/listings/linking_prerequisites.c`
  `tail -n +10 $srcdir/docs/listings/linking_safe.c.in > $PWD/docs/listings/linking_safe.c`
  `tail -n +10 $srcdir/docs/listings/linking_unsafe.c.in > $PWD/docs/listings/linking_unsafe.c`
  `tail -n +10 $srcdir/docs/listings/notation_clipboard.c.in > $PWD/docs/listings/notation_clipboard.c`
  `tail -n +10 $srcdir/docs/listings/pcm_info.c.in > $PWD/docs/listings/pcm_info.c`
  `tail -n +10 $srcdir/docs/listings/port.c.in > $PWD/docs/listings/port.c`
  `tail -n +10 $srcdir/docs/listings/pull_thread.c.in > $PWD/docs/listings/pull_thread.c`
  `tail -n +10 $srcdir/docs/listings/recycling.c.in > $PWD/docs/listings/recycling.c`
  `tail -n +10 $srcdir/docs/listings/start_thread.c.in > $PWD/docs/listings/start_thread.c`
  `tail -n +10 $srcdir/docs/listings/thread_application_context.c.in > $PWD/docs/listings/thread_application_context.c`
  `tail -n +10 $srcdir/docs/listings/thread_obj_mutex.c.in > $PWD/docs/listings/thread_obj_mutex.c`
])
