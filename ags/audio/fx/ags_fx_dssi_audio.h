/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#ifndef __AGS_FX_DSSI_AUDIO_H__
#define __AGS_FX_DSSI_AUDIO_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/fx/ags_fx_notation_audio.h>

G_BEGIN_DECLS

#define AGS_TYPE_FX_DSSI_AUDIO                (ags_fx_dssi_audio_get_type())
#define AGS_FX_DSSI_AUDIO(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_FX_DSSI_AUDIO, AgsFxDssiAudio))
#define AGS_FX_DSSI_AUDIO_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_FX_DSSI_AUDIO, AgsFxDssiAudio))
#define AGS_IS_FX_DSSI_AUDIO(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_FX_DSSI_AUDIO))
#define AGS_IS_FX_DSSI_AUDIO_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_FX_DSSI_AUDIO))
#define AGS_FX_DSSI_AUDIO_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_FX_DSSI_AUDIO, AgsFxDssiAudioClass))

typedef enum{
  AGS_FX_DSSI_AUDIO_LIVE_INSTRUMENT     = 1,
}AgsFxNotationAudioFlags;

struct _AgsFxDssiAudio
{
  AgsFxNotationAudio fx_notation_audio;

  guint flags;
  
  guint input_lines;
  guint output_lines;

  guint *input_port;
  guint *output_port;

  guint bank;
  guint program;

  LADSPA_Data* input[128];
  LADSPA_Data* output[128];
  
  guint event_count;
  snd_seq_event_t event_buffer[128];

  LADSPA_Handle* ladspa_handle[128];

  AgsDssiPlugin *dssi_plugin;
};

struct _AgsFxDssiAudioClass
{
  AgsFxNotationAudioClass fx_notation_audio;
};

GType ags_fx_dssi_audio_get_type();

gboolean ags_fx_dssi_audio_test_flags(AgsFxDssiAudio *fx_dssi_audio);
void ags_fx_dssi_audio_set_flags(AgsFxDssiAudio *fx_dssi_audio, guint flags);
void ags_fx_dssi_audio_unset_flags(AgsFxDssiAudio *fx_dssi_audio, guint flags);

void ags_fx_dssi_audio_load_plugin(AgsFxDssiAudio *fx_dssi_audio);

AgsFxDssiAudio* ags_fx_dssi_audio_new(AgsAudio *audio);

G_END_DECLS

#endif /*__AGS_FX_DSSI_AUDIO_H__*/

