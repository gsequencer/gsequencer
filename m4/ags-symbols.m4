# -*- mode: autoconf -*-
#
# GSequencer - Advanced GTK Sequencer 
# Copyright (C) 2018 Joël Krähemann
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

AC_DEFUN([LIBAGS_SYMBOLS],
[
  AC_MSG_NOTICE([generating libags.sym])
  `tail -n +19 $srcdir/libags.sym.in > $PWD/libags.sym`
])

AC_DEFUN([LIBAGS_THREAD_SYMBOLS],
[
  AC_MSG_NOTICE([generating libags_thread.sym])
  `tail -n +19 $srcdir/libags_thread.sym.in > $PWD/libags_thread.sym`
])

AC_DEFUN([LIBAGS_SERVER_SYMBOLS],
[
  AC_MSG_NOTICE([generating libags_server.sym])
  `tail -n +19 $srcdir/libags_server.sym.in > $PWD/libags_server.sym`
])

AC_DEFUN([LIBAGS_AUDIO_SYMBOLS],
[
  AC_MSG_NOTICE([generating libags_audio.sym])
  `tail -n +19 $srcdir/libags_audio.sym.in > $PWD/libags_audio.sym`
])

AC_DEFUN([LIBAGS_GUI_SYMBOLS],
[
  AC_MSG_NOTICE([generating libags_gui.sym])
  `tail -n +19 $srcdir/libags_gui.sym.in > $PWD/libags_gui.sym`
])
