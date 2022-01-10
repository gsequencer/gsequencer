/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
 *
 * This file is part of GSequencer.
 *
 * GSequencer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GSequencer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GSequencer.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __AGS_FX_SF2_SYNTH_AUDIO_SIGNAL_H__
#define __AGS_FX_SF2_SYNTH_AUDIO_SIGNAL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio_signal.h>

#include <ags/audio/fx/ags_fx_notation_audio_signal.h>

G_BEGIN_DECLS

#define AGS_TYPE_FX_SF2_SYNTH_AUDIO_SIGNAL                (ags_fx_sf2_synth_audio_signal_get_type())
#define AGS_FX_SF2_SYNTH_AUDIO_SIGNAL(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_FX_SF2_SYNTH_AUDIO_SIGNAL, AgsFxSF2SynthAudioSignal))
#define AGS_FX_SF2_SYNTH_AUDIO_SIGNAL_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_FX_SF2_SYNTH_AUDIO_SIGNAL, AgsFxSF2SynthAudioSignalClass))
#define AGS_IS_FX_SF2_SYNTH_AUDIO_SIGNAL(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_FX_SF2_SYNTH_AUDIO_SIGNAL))
#define AGS_IS_FX_SF2_SYNTH_AUDIO_SIGNAL_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_FX_SF2_SYNTH_AUDIO_SIGNAL))
#define AGS_FX_SF2_SYNTH_AUDIO_SIGNAL_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_FX_SF2_SYNTH_AUDIO_SIGNAL, AgsFxSF2SynthAudioSignalClass))

typedef struct _AgsFxSF2SynthAudioSignal AgsFxSF2SynthAudioSignal;
typedef struct _AgsFxSF2SynthAudioSignalClass AgsFxSF2SynthAudioSignalClass;

struct _AgsFxSF2SynthAudioSignal
{
  AgsFxNotationAudioSignal fx_notation_audio_signal;
};

struct _AgsFxSF2SynthAudioSignalClass
{
  AgsFxNotationAudioSignalClass fx_notation_audio_signal;
};

GType ags_fx_sf2_synth_audio_signal_get_type();

/*  */
AgsFxSF2SynthAudioSignal* ags_fx_sf2_synth_audio_signal_new(AgsAudioSignal *audio_signal);

G_END_DECLS

#endif /*__AGS_FX_SF2_SYNTH_AUDIO_SIGNAL_H__*/
