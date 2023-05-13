# -*- mode: autoconf -*-
#
# GSequencer - Advanced GTK Sequencer 
# Copyright (C) 2005-2023 Joël Krähemann
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
  `mkdir -p $PWD/docs/reference/libags/`
  `cp -n $srcdir/docs/reference/libags/libags.xml $PWD/docs/reference/libags/ &2> /dev/null`
  `tail -n +19 $srcdir/docs/reference/libags/libags-sections.txt.in > $PWD/docs/reference/libags/libags-sections.txt`
  `tail -n +19 $srcdir/docs/reference/libags/libags.types.in > $PWD/docs/reference/libags/libags.types`
  `tail -n +19 $srcdir/docs/reference/libags/libags.interfaces.in > $PWD/docs/reference/libags/libags.interfaces`
  `mkdir -p $PWD/docs/reference/libags-audio`
  `cp -n $srcdir/docs/reference/libags-audio/libags_audio.xml $PWD/docs/reference/libags-audio/ &2> /dev/null`
  `tail -n +19 $srcdir/docs/reference/libags-audio/libags_audio-sections.txt.in > $PWD/docs/reference/libags-audio/libags_audio-sections.txt`
  `tail -n +19 $srcdir/docs/reference/libags-audio/libags_audio.types.in > $PWD/docs/reference/libags-audio/libags_audio.types`
  AS_IF([test "x$enable_libinstpatch" != xno],
     `echo "ags_apply_sf2_midi_locale_get_type" >> $PWD/docs/reference/libags-audio/libags_audio.types`
     `echo "ags_apply_sf2_synth_get_type" >> $PWD/docs/reference/libags-audio/libags_audio.types`
     `echo "ags_fx_sf2_synth_audio_get_type" >> $PWD/docs/reference/libags-audio/libags_audio.types`
     `echo "ags_fx_sf2_synth_audio_processor_get_type" >> $PWD/docs/reference/libags-audio/libags_audio.types`
     `echo "ags_fx_sf2_synth_audio_signal_get_type" >> $PWD/docs/reference/libags-audio/libags_audio.types`
     `echo "ags_fx_sf2_synth_channel_get_type" >> $PWD/docs/reference/libags-audio/libags_audio.types`
     `echo "ags_fx_sf2_synth_channel_processor_get_type" >> $PWD/docs/reference/libags-audio/libags_audio.types`
     `echo "ags_fx_sf2_synth_recycling_get_type" >> $PWD/docs/reference/libags-audio/libags_audio.types`
     `echo "ags_ipatch_dls2_reader_get_type" >> $PWD/docs/reference/libags-audio/libags_audio.types`
     `echo "ags_ipatch_flags_get_type" >> $PWD/docs/reference/libags-audio/libags_audio.types`
     `echo "ags_ipatch_get_type" >> $PWD/docs/reference/libags-audio/libags_audio.types`
     `echo "ags_ipatch_gig_reader_get_type" >> $PWD/docs/reference/libags-audio/libags_audio.types`
     `echo "ags_ipatch_sample_get_type" >> $PWD/docs/reference/libags-audio/libags_audio.types`
     `echo "ags_ipatch_sf2_reader_get_type" >> $PWD/docs/reference/libags-audio/libags_audio.types`
     `echo "ags_open_sf2_instrument_get_type" >> $PWD/docs/reference/libags-audio/libags_audio.types`
     `echo "ags_open_sf2_sample_get_type" >> $PWD/docs/reference/libags-audio/libags_audio.types`
     `echo "ags_sf2_loader_get_type" >> $PWD/docs/reference/libags-audio/libags_audio.types`
     `echo "ags_sf2_midi_locale_loader_get_type" >> $PWD/docs/reference/libags-audio/libags_audio.types`
     `echo "ags_sf2_synth_generator_get_type" >> $PWD/docs/reference/libags-audio/libags_audio.types`
     `echo "ags_sf2_synth_util_get_type" >> $PWD/docs/reference/libags-audio/libags_audio.types`
     `echo "ags_sf2_synth_util_loop_mode_get_type" >> $PWD/docs/reference/libags-audio/libags_audio.types`
     )
  AS_IF([test "x$enable_vst3" != xno],
     `echo "ags_fx_vst3_audio_get_type" >> $PWD/docs/reference/libags-audio/libags_audio.types`
     `echo "ags_fx_vst3_audio_processor_get_type" >> $PWD/docs/reference/libags-audio/libags_audio.types`
     `echo "ags_fx_vst3_audio_signal_get_type" >> $PWD/docs/reference/libags-audio/libags_audio.types`
     `echo "ags_fx_vst3_channel_get_type" >> $PWD/docs/reference/libags-audio/libags_audio.types`
     `echo "ags_fx_vst3_channel_processor_get_type" >> $PWD/docs/reference/libags-audio/libags_audio.types`
     `echo "ags_fx_vst3_recycling_get_type" >> $PWD/docs/reference/libags-audio/libags_audio.types`
     `echo "ags_instantiate_vst3_plugin_get_type" >> $PWD/docs/reference/libags-audio/libags_audio.types`
     `echo "ags_vst3_conversion_get_type" >> $PWD/docs/reference/libags-audio/libags_audio.types`
     `echo "ags_vst3_manager_get_type" >> $PWD/docs/reference/libags-audio/libags_audio.types`
     `echo "ags_vst3_plugin_get_type" >> $PWD/docs/reference/libags-audio/libags_audio.types`
     `echo "ags_write_vst3_port_get_type" >> $PWD/docs/reference/libags-audio/libags_audio.types`
     )
  `tail -n +19 $srcdir/docs/reference/libags-audio/libags_audio.interfaces.in > $PWD/docs/reference/libags-audio/libags_audio.interfaces`
  `mkdir -p $PWD/docs/reference/libags-gui`
  `cp -n $srcdir/docs/reference/libags-gui/libags_gui.xml $PWD/docs/reference/libags-gui/ &2> /dev/null`
  `tail -n +19 $srcdir/docs/reference/libags-gui/libags_gui-sections.txt.in > $PWD/docs/reference/libags-gui/libags_gui-sections.txt`
  `tail -n +19 $srcdir/docs/reference/libags-gui/libags_gui.types.in > $PWD/docs/reference/libags-gui/libags_gui.types`
  `tail -n +19 $srcdir/docs/reference/libags-gui/libags_gui.interfaces.in > $PWD/docs/reference/libags-gui/libags_gui.interfaces`
  `mkdir -p $PWD/docs/reference/libgsequencer`
  `cp -n $srcdir/docs/reference/libgsequencer/libgsequencer.xml $PWD/docs/reference/libgsequencer/ &2> /dev/null`
  `tail -n +19 $srcdir/docs/reference/libgsequencer/libgsequencer-sections.txt.in > $PWD/docs/reference/libgsequencer/libgsequencer-sections.txt`
  `tail -n +19 $srcdir/docs/reference/libgsequencer/libgsequencer.types.in > $PWD/docs/reference/libgsequencer/libgsequencer.types`
  AS_IF([test "x$enable_libinstpatch" != xno],
     `echo "ags_sf2_synth_get_type" >> $PWD/docs/reference/libgsequencer/libgsequencer.types`
     `echo "ags_ffplayer_bridge_get_type" >> $PWD/docs/reference/libgsequencer/libgsequencer.types`
     `echo "ags_ffplayer_bulk_input_get_type" >> $PWD/docs/reference/libgsequencer/libgsequencer.types`
     `echo "ags_ffplayer_get_type" >> $PWD/docs/reference/libgsequencer/libgsequencer.types`
     `echo "ags_ffplayer_input_line_get_type" >> $PWD/docs/reference/libgsequencer/libgsequencer.types`
     `echo "ags_ffplayer_input_pad_get_type" >> $PWD/docs/reference/libgsequencer/libgsequencer.types`
     )
  AS_IF([test "x$enable_vst3" != xno],
     `echo "ags_live_vst3_bridge_get_type" >> $PWD/docs/reference/libgsequencer/libgsequencer.types`
     `echo "ags_vst3_bridge_get_type" >> $PWD/docs/reference/libgsequencer/libgsequencer.types`
     `echo "ags_vst3_browser_get_type" >> $PWD/docs/reference/libgsequencer/libgsequencer.types`
     )
  `tail -n +19 $srcdir/docs/reference/libgsequencer/libgsequencer.interfaces.in > $PWD/docs/reference/libgsequencer/libgsequencer.interfaces`
])
