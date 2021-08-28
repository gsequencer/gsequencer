/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
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

#ifndef __AGS_FX_VST3_AUDIO_H__
#define __AGS_FX_VST3_AUDIO_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>
#include <ags/libags-vst.h>

#include <alsa/seq_event.h>

#include <ags/plugin/ags_vst3_plugin.h>

#include <ags/audio/ags_port.h>

#include <ags/audio/fx/ags_fx_notation_audio.h>

G_BEGIN_DECLS

#define AGS_TYPE_FX_VST3_AUDIO                (ags_fx_vst3_audio_get_type())
#define AGS_FX_VST3_AUDIO(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_FX_VST3_AUDIO, AgsFxVst3Audio))
#define AGS_FX_VST3_AUDIO_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_FX_VST3_AUDIO, AgsFxVst3AudioClass))
#define AGS_IS_FX_VST3_AUDIO(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_FX_VST3_AUDIO))
#define AGS_IS_FX_VST3_AUDIO_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_FX_VST3_AUDIO))
#define AGS_FX_VST3_AUDIO_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_FX_VST3_AUDIO, AgsFxVst3AudioClass))

#define AGS_FX_VST3_AUDIO_SCOPE_DATA(ptr) ((AgsFxVst3AudioScopeData *) (ptr))
#define AGS_FX_VST3_AUDIO_SCOPE_DATA_GET_STRCT_MUTEX(ptr) (&(((AgsFxVst3AudioScopeData *)(ptr))->strct_mutex))

#define AGS_FX_VST3_AUDIO_CHANNEL_DATA(ptr) ((AgsFxVst3AudioChannelData *) (ptr))
#define AGS_FX_VST3_AUDIO_CHANNEL_DATA_GET_STRCT_MUTEX(ptr) (&(((AgsFxVst3AudioChannelData *)(ptr))->strct_mutex))

#define AGS_FX_VST3_AUDIO_INPUT_DATA(ptr) ((AgsFxVst3AudioInputData *) (ptr))
#define AGS_FX_VST3_AUDIO_INPUT_DATA_GET_STRCT_MUTEX(ptr) (&(((AgsFxVst3AudioInputData *)(ptr))->strct_mutex))

#define AGS_FX_VST3_AUDIO_DEFAULT_MIDI_LENGHT (8 * 256)

typedef struct _AgsFxVst3Audio AgsFxVst3Audio;
typedef struct _AgsFxVst3AudioScopeData AgsFxVst3AudioScopeData;
typedef struct _AgsFxVst3AudioChannelData AgsFxVst3AudioChannelData;
typedef struct _AgsFxVst3AudioInputData AgsFxVst3AudioInputData;
typedef struct _AgsFxVst3AudioClass AgsFxVst3AudioClass;

typedef enum{
  AGS_FX_VST3_AUDIO_LIVE_INSTRUMENT     = 1,
}AgsFxVst3AudioFlags;

struct _AgsFxVst3Audio
{
  AgsFxNotationAudio fx_notation_audio;

  guint flags;
  
  guint output_port_count;
  guint *output_port;

  guint input_port_count;
  guint *input_port;
  
  AgsFxVst3AudioScopeData* scope_data[AGS_SOUND_SCOPE_LAST];

  AgsVst3Plugin *vst3_plugin;

  AgsPort **vst3_port;
};

struct _AgsFxVst3AudioClass
{
  AgsFxNotationAudioClass fx_notation_audio;
};

struct _AgsFxVst3AudioScopeData
{
  GRecMutex strct_mutex;
  
  gpointer parent;
  
  guint audio_channels;
  
  AgsFxVst3AudioChannelData **channel_data;
};

struct _AgsFxVst3AudioChannelData
{
  GRecMutex strct_mutex;
  
  gpointer parent;

  guint event_count;
  
  float *output;
  float *input;
  
  AgsVstIComponent *icomponent;
  AgsVstIEditController *iedit_controller;

  AgsFxVst3AudioInputData* input_data[AGS_SEQUENCER_MAX_MIDI_KEYS];
};

struct _AgsFxVst3AudioInputData
{
  GRecMutex strct_mutex;
  
  gpointer parent;

  float *output;
  float *input;

  AgsVstIComponent *icomponent;
  AgsVstIEditController *iedit_controller;

  snd_seq_event_t *event_buffer;
  guint key_on;
};

GType ags_fx_vst3_audio_get_type();

/* runtime */
AgsFxVst3AudioScopeData* ags_fx_vst3_audio_scope_data_alloc();
void ags_fx_vst3_audio_scope_data_free(AgsFxVst3AudioScopeData *scope_data);

AgsFxVst3AudioChannelData* ags_fx_vst3_audio_channel_data_alloc();
void ags_fx_vst3_audio_channel_data_free(AgsFxVst3AudioChannelData *channel_data);

AgsFxVst3AudioInputData* ags_fx_vst3_audio_input_data_alloc();
void ags_fx_vst3_audio_input_data_free(AgsFxVst3AudioInputData *input_data);

/* flags */
gboolean ags_fx_vst3_audio_test_flags(AgsFxVst3Audio *fx_vst3_audio, guint flags);
void ags_fx_vst3_audio_set_flags(AgsFxVst3Audio *fx_vst3_audio, guint flags);
void ags_fx_vst3_audio_unset_flags(AgsFxVst3Audio *fx_vst3_audio, guint flags);

/* load/unload */
void ags_fx_vst3_audio_load_plugin(AgsFxVst3Audio *fx_vst3_audio);
void ags_fx_vst3_audio_load_port(AgsFxVst3Audio *fx_vst3_audio);

/* instantiate */
AgsFxVst3Audio* ags_fx_vst3_audio_new(AgsAudio *audio);

G_END_DECLS

#endif /*__AGS_FX_VST3_AUDIO_H__*/
