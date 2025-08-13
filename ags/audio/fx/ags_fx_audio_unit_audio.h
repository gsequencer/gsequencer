/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2025 Joël Krähemann
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

#ifndef __AGS_FX_AUDIO_UNIT_AUDIO_H__
#define __AGS_FX_AUDIO_UNIT_AUDIO_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/plugin/ags_audio_unit_plugin.h>

#include <ags/audio/ags_port.h>

#include <ags/audio/fx/ags_fx_notation_audio.h>

G_BEGIN_DECLS

#define AGS_TYPE_FX_AUDIO_UNIT_AUDIO                (ags_fx_audio_unit_audio_get_type())
#define AGS_FX_AUDIO_UNIT_AUDIO(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_FX_AUDIO_UNIT_AUDIO, AgsFxAudioUnitAudio))
#define AGS_FX_AUDIO_UNIT_AUDIO_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_FX_AUDIO_UNIT_AUDIO, AgsFxAudioUnitAudioClass))
#define AGS_IS_FX_AUDIO_UNIT_AUDIO(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_FX_AUDIO_UNIT_AUDIO))
#define AGS_IS_FX_AUDIO_UNIT_AUDIO_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_FX_AUDIO_UNIT_AUDIO))
#define AGS_FX_AUDIO_UNIT_AUDIO_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_FX_AUDIO_UNIT_AUDIO, AgsFxAudioUnitAudioClass))

#define AGS_FX_AUDIO_UNIT_AUDIO_SCOPE_DATA(ptr) ((AgsFxAudioUnitAudioScopeData *) (ptr))
#define AGS_FX_AUDIO_UNIT_AUDIO_SCOPE_DATA_GET_STRCT_MUTEX(ptr) (&(((AgsFxAudioUnitAudioScopeData *)(ptr))->strct_mutex))

typedef struct _AgsFxAudioUnitAudio AgsFxAudioUnitAudio;
typedef struct _AgsFxAudioUnitAudioScopeData AgsFxAudioUnitAudioScopeData;
typedef struct _AgsFxAudioUnitAudioChannelData AgsFxAudioUnitAudioChannelData;
typedef struct _AgsFxAudioUnitAudioInputData AgsFxAudioUnitAudioInputData;
typedef struct _AgsFxAudioUnitAudioClass AgsFxAudioUnitAudioClass;

struct _AgsFxAudioUnitAudio
{
  AgsFxNotationAudio fx_notation_audio;

  guint flags;
    
  AgsAudioUnitPlugin *audio_unit_plugin;

  gpointer av_format;
  
  gpointer audio_engine;

  gpointer av_audio_player_node;
  gpointer av_audio_unit;
  gpointer av_audio_sequencer;
  
  gboolean render_thread_running;
  
  GThread *render_thread;
  
  AgsFxAudioUnitAudioScopeData* scope_data[AGS_SOUND_SCOPE_LAST];
};

struct _AgsFxAudioUnitAudioClass
{
  AgsFxNotationAudioClass fx_notation_audio;
};

struct _AgsFxAudioUnitAudioScopeData
{
  GRecMutex strct_mutex;
  
  gpointer parent;
  
  guint audio_channels;

  float *output;
  guint output_buffer_size;
  
  float *input;
  guint input_buffer_size;

  gpointer av_output_buffer;
  gpointer av_input_buffer;

  _Atomic gboolean completed_wait;
  _Atomic gint active_audio_signal_count;
  _Atomic gint completed_audio_signal_count;

  GMutex completed_mutex;
  GCond completed_cond;

  GHashTable *active_audio_signal;
  
  GHashTable *written_audio_signal;

  _Atomic gboolean render_wait;
  _Atomic gboolean render_done;

  GMutex render_mutex;
  GCond render_cond;
  
  AgsFxAudioUnitAudioChannelData* channel_data[1];
};

struct _AgsFxAudioUnitAudioChannelData
{
  GRecMutex strct_mutex;
  
  gpointer parent;

  guint event_count;

  AgsFxAudioUnitAudioInputData* input_data[AGS_SEQUENCER_MAX_MIDI_KEYS];
};

struct _AgsFxAudioUnitAudioInputData
{
  GRecMutex strct_mutex;
  
  gpointer parent;

  gpointer av_music_track;

  GList *note;
  
  guint key_on;
};

GType ags_fx_audio_unit_audio_get_type();

/* runtime */
AgsFxAudioUnitAudioScopeData* ags_fx_audio_unit_audio_scope_data_alloc();
void ags_fx_audio_unit_audio_scope_data_free(AgsFxAudioUnitAudioScopeData *scope_data);

AgsFxAudioUnitAudioChannelData* ags_fx_audio_unit_audio_channel_data_alloc();
void ags_fx_audio_unit_audio_channel_data_free(AgsFxAudioUnitAudioChannelData *channel_data);

AgsFxAudioUnitAudioInputData* ags_fx_audio_unit_audio_input_data_alloc();
void ags_fx_audio_unit_audio_input_data_free(AgsFxAudioUnitAudioInputData *input_data);

/* flags */
gboolean ags_fx_audio_unit_audio_test_flags(AgsFxAudioUnitAudio *fx_audio_unit_audio, guint flags);
void ags_fx_audio_unit_audio_set_flags(AgsFxAudioUnitAudio *fx_audio_unit_audio, guint flags);
void ags_fx_audio_unit_audio_unset_flags(AgsFxAudioUnitAudio *fx_audio_unit_audio, guint flags);

/* load/unload */
void ags_fx_audio_unit_audio_load_plugin(AgsFxAudioUnitAudio *fx_audio_unit_audio);
void ags_fx_audio_unit_audio_load_port(AgsFxAudioUnitAudio *fx_audio_unit_audio);

/* render thread */
void ags_fx_audio_unit_audio_start_render_thread(AgsFxAudioUnitAudio *fx_audio_unit_audio);
void ags_fx_audio_unit_audio_stop_render_thread(AgsFxAudioUnitAudio *fx_audio_unit_audio);

/* instantiate */
AgsFxAudioUnitAudio* ags_fx_audio_unit_audio_new(AgsAudio *audio);

G_END_DECLS

#endif /*__AGS_FX_AUDIO_UNIT_AUDIO_H__*/
