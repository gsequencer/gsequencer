# -*- mode: autoconf -*-
#
# GSequencer - Advanced GTK Sequencer 
# Copyright (C) 2019 Joël Krähemann
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
  `mkdir -p docs/listings/`
  `tail -n +10 $srcdir/docs/listings/add_pattern.c.in > docs/listings/add_pattern.c`
  `tail -n +10 $srcdir/docs/listings/application_mutex.c.in > docs/listings/application_mutex.c`
  `tail -n +10 $srcdir/docs/listings/audio.c.in > docs/listings/audio.c`
  `tail -n +10 $srcdir/docs/listings/audio_application_context.c.in > docs/listings/audio_application_context.c`
  `tail -n +10 $srcdir/docs/listings/complete_example.c.in > docs/listings/complete_example.c`
  `tail -n +10 $srcdir/docs/listings/config.c.in > docs/listings/config.c`
  `tail -n +10 $srcdir/docs/listings/effects_echo_audio.c.in > docs/listings/effects_echo_audio.c`
  `tail -n +10 $srcdir/docs/listings/effects_echo_audio_run.c.in > docs/listings/effects_echo_audio_run.c`
  `tail -n +10 $srcdir/docs/listings/effects_echo_channel.c.in > docs/listings/effects_echo_channel.c`
  `tail -n +10 $srcdir/docs/listings/effects_echo_channel_run.c.in > docs/listings/effects_echo_channel_run.c`
  `tail -n +10 $srcdir/docs/listings/effects_recall_container.c.in > docs/listings/effects_recall_container.c`
  `tail -n +10 $srcdir/docs/listings/file_property.c.in > docs/listings/file_property.c`
  `tail -n +10 $srcdir/docs/listings/file_read.c.in > docs/listings/file_read.c`
  `tail -n +10 $srcdir/docs/listings/file_write.c.in > docs/listings/file_write.c`
  `tail -n +10 $srcdir/docs/listings/notation_clipboard.c.in > docs/listings/notation_clipboard.c`
  `tail -n +10 $srcdir/docs/listings/linking_prerequisites.c.in > docs/listings/linking_prerequisites.c`
  `tail -n +10 $srcdir/docs/listings/linking_safe.c.in > docs/listings/linking_safe.c`
  `tail -n +10 $srcdir/docs/listings/linking_unsafe.c.in > docs/listings/linking_unsafe.c`
  `tail -n +10 $srcdir/docs/listings/pcm_info.c.in > docs/listings/pcm_info.c`
  `tail -n +10 $srcdir/docs/listings/port.c.in > docs/listings/port.c`
  `tail -n +10 $srcdir/docs/listings/pull_thread.c.in > docs/listings/pull_thread.c`
  `tail -n +10 $srcdir/docs/listings/recycling.c.in > docs/listings/recycling.c`
  `tail -n +10 $srcdir/docs/listings/start_thread.c.in > docs/listings/start_thread.c`
  `tail -n +10 $srcdir/docs/listings/thread_application_context.c.in > docs/listings/thread_application_context.c`
  `tail -n +10 $srcdir/docs/listings/thread_obj_mutex.c.in > docs/listings/thread_obj_mutex.c`
])
