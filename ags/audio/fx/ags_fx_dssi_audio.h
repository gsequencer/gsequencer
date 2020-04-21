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

#include <ags/libags.h>

#include <alsa/seq_event.h>

#include <ladspa.h>
#include <dssi.h>

#include <ags/plugin/ags_dssi_plugin.h>

#include <ags/audio/ags_port.h>
#include <ags/audio/ags_sound_enums.h>

#include <ags/audio/fx/ags_fx_notation_audio.h>

G_BEGIN_DECLS

#define AGS_TYPE_FX_DSSI_AUDIO                (ags_fx_dssi_audio_get_type())
#define AGS_FX_DSSI_AUDIO(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_FX_DSSI_AUDIO, AgsFxDssiAudio))
#define AGS_FX_DSSI_AUDIO_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_FX_DSSI_AUDIO, AgsFxDssiAudio))
#define AGS_IS_FX_DSSI_AUDIO(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_FX_DSSI_AUDIO))
#define AGS_IS_FX_DSSI_AUDIO_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_FX_DSSI_AUDIO))
#define AGS_FX_DSSI_AUDIO_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_FX_DSSI_AUDIO, AgsFxDssiAudioClass))

#define AGS_FX_DSSI_AUDIO_SCOPE_DATA(ptr) ((AgsFxDssiAudioScopeData *) (ptr))
#define AGS_FX_DSSI_AUDIO_CHANNEL_DATA(ptr) ((AgsFxDssiAudioChannelData *) (ptr))
#define AGS_FX_DSSI_AUDIO_INPUT_DATA(ptr) ((AgsFxDssiAudioInputData *) (ptr))

typedef struct _AgsFxDssiAudio AgsFxDssiAudio;
typedef struct _AgsFxDssiAudioScopeData AgsFxDssiAudioScopeData;
typedef struct _AgsFxDssiAudioChannelData AgsFxDssiAudioChannelData;
typedef struct _AgsFxDssiAudioInputData AgsFxDssiAudioInputData;
typedef struct _AgsFxDssiAudioClass AgsFxDssiAudioClass;

typedef enum{
  AGS_FX_DSSI_AUDIO_LIVE_INSTRUMENT     = 1,
}AgsFxDssiAudioFlags;

struct _AgsFxDssiAudio
{
  AgsFxNotationAudio fx_notation_audio;

  guint flags;

  guint output_port_count;
  guint *output_port;

  guint input_port_count;
  guint *input_port;

  guint bank;
  guint program;

  AgsFxDssiAudioScopeData* scope_data[AGS_SOUND_SCOPE_LAST];

  AgsDssiPlugin *dssi_plugin;

  AgsPort **dssi_port;
};

struct _AgsFxDssiAudioClass
{
  AgsFxNotationAudioClass fx_notation_audio;
};

GType ags_fx_dssi_audio_get_type();

struct _AgsFxDssiAudioScopeData
{
  gpointer parent;
  
  guint audio_channels;
  
  AgsFxDssiAudioChannelData **channel_data;
};

struct _AgsFxDssiAudioChannelData
{
  gpointer parent;

  guint event_count;
  
  LADSPA_Data *output;
  LADSPA_Data *input;
  
  LADSPA_Handle ladspa_handle;

  AgsFxDssiAudioInputData* input_data[128];
};

struct _AgsFxDssiAudioInputData
{
  gpointer parent;

  LADSPA_Data *output;
  LADSPA_Data *input;

  LADSPA_Handle ladspa_handle;
  
  snd_seq_event_t *event_buffer;
  guint key_on;
};

/* runtime */
AgsFxDssiAudioScopeData* ags_fx_dssi_audio_scope_data_alloc();
void ags_fx_dssi_audio_scope_data_free(AgsFxDssiAudioScopeData *scope_data);

AgsFxDssiAudioChannelData* ags_fx_dssi_audio_channel_data_alloc();
void ags_fx_dssi_audio_channel_data_free(AgsFxDssiAudioChannelData *channel_data);

AgsFxDssiAudioInputData* ags_fx_dssi_audio_input_data_alloc();
void ags_fx_dssi_audio_input_data_free(AgsFxDssiAudioInputData *input_data);

/* flags */
gboolean ags_fx_dssi_audio_test_flags(AgsFxDssiAudio *fx_dssi_audio, guint flags);
void ags_fx_dssi_audio_set_flags(AgsFxDssiAudio *fx_dssi_audio, guint flags);
void ags_fx_dssi_audio_unset_flags(AgsFxDssiAudio *fx_dssi_audio, guint flags);

/* load/unload */
void ags_fx_dssi_audio_load_plugin(AgsFxDssiAudio *fx_dssi_audio);
void ags_fx_dssi_audio_load_port(AgsFxDssiAudio *fx_dssi_audio);

/* connect port */
void ags_fx_dssi_audio_connect_port(AgsFxDssiAudio *fx_dssi_audio);

/* plugin */
void ags_fx_dssi_audio_change_program(AgsFxDssiAudio *fx_dssi_audio,
				      guint bank_index,
				      guint program_index);

/* instantiate */
AgsFxDssiAudio* ags_fx_dssi_audio_new(AgsAudio *audio);

G_END_DECLS

#endif /*__AGS_FX_DSSI_AUDIO_H__*/
