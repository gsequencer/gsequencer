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

define(LIBAGS_SYMBOLS, syscmd(tail -n +17 libags.sym.in > libags.sym))
define(LIBAGS_THREAD_SYMBOLS, syscmd(tail -n +17 libags_thread.sym.in > libags_thread.sym))
define(LIBAGS_SERVER_SYMBOLS, syscmd(tail -n +17 libags_server.sym.in > libags_server.sym))
define(LIBAGS_AUDIO_SYMBOLS, syscmd(tail -n +17 libags_audio.sym.in > libags_audio.sym))
define(LIBAGS_GUI_SYMBOLS, syscmd(tail -n +17 libags_gui.sym.in > libags_gui.sym))
