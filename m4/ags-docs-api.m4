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

AC_DEFUN([AGS_DOCS_API],
[
  AC_MSG_NOTICE([generating resources for API Reference Manual])
  `tail -n +19 $srcdir/docs/reference/libags/libags-sections.txt.in > $srcdir/docs/reference/libags/libags-sections.txt`
  `tail -n +19 $srcdir/docs/reference/libags/libags.types.in > $srcdir/docs/reference/libags/libags.types`
  `tail -n +19 $srcdir/docs/reference/libags/libags.interfaces.in > $srcdir/docs/reference/libags/libags.interfaces`
  `tail -n +19 $srcdir/docs/reference/libags-audio/libags_audio-sections.txt.in > $srcdir/docs/reference/libags-audio/libags_audio-sections.txt`
  `tail -n +19 $srcdir/docs/reference/libags-audio/libags_audio.types.in > $srcdir/docs/reference/libags-audio/libags_audio.types`
  `tail -n +19 $srcdir/docs/reference/libags-audio/libags_audio.interfaces.in > $srcdir/docs/reference/libags-audio/libags_audio.interfaces`
  `tail -n +19 $srcdir/docs/reference/libags-gui/libags_gui-sections.txt.in > $srcdir/docs/reference/libags-gui/libags_gui-sections.txt`
  `tail -n +19 $srcdir/docs/reference/libags-gui/libags_gui.types.in > $srcdir/docs/reference/libags-gui/libags_gui.types`
  `tail -n +19 $srcdir/docs/reference/libags-gui/libags_gui.interfaces.in > $srcdir/docs/reference/libags-gui/libags_gui.interfaces`
  `tail -n +19 $srcdir/docs/reference/libgsequencer/libgsequencer-sections.txt.in > $srcdir/docs/reference/libgsequencer/libgsequencer-sections.txt`
  `tail -n +19 $srcdir/docs/reference/libgsequencer/libgsequencer.types.in > $srcdir/docs/reference/libgsequencer/libgsequencer.types`
  `tail -n +19 $srcdir/docs/reference/libgsequencer/libgsequencer.interfaces.in > $srcdir/docs/reference/libgsequencer/libgsequencer.interfaces`
])
