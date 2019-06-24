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

AC_DEFUN([LIBAGS_MARSHALLERS],
[
  AC_MSG_NOTICE([generating ags/object/ags_marshallers.list])
  `tail -n +19 $srcdir/ags/object/ags_marshallers.list.in > $srcdir/ags/object/ags_marshallers.list`
])

AC_DEFUN([LIBAGS_GUI_MARSHALLERS],
[
  AC_MSG_NOTICE([generating ags/widget/ags_widget_marshallers.list])
  `tail -n +19 $srcdir/ags/widget/ags_widget_marshallers.list.in > $srcdir/ags/widget/ags_widget_marshallers.list`
])
