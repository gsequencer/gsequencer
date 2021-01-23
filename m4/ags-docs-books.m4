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

AC_DEFUN([AGS_DOCS_BOOKS],
[
  AC_MSG_NOTICE([generating docs/listings/*])
  `mkdir -p $PWD/html`
  `mkdir -p $PWD/html/user-manual`
  `mkdir -p $PWD/html/developer-manual`
  `mkdir -p $PWD/html/osc-manual`
  `mkdir -p $PWD/pdf`
  `cp -n $srcdir/docs/usersBook.xml $PWD/docs/ &2> /dev/null`
  `cp -n $srcdir/docs/developersBook.xml $PWD/docs/ &2> /dev/null`
  `cp -n $srcdir/docs/oscBook.xml $PWD/docs/ &2> /dev/null`
  `cp -n $srcdir/docs/appa.xml $PWD/docs/ &2> /dev/null`
  `cp $srcdir/docs/custom.css $PWD/html/user-manual/`
  `cp $srcdir/docs/custom.css $PWD/html/developer-manual/`
  `cp $srcdir/docs/custom.css $PWD/html/osc-manual/`
])
