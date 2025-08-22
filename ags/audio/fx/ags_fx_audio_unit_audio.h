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

#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_port.h>
#include <ags/audio/ags_volume_util.h>

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

#define AGS_FX_AUDIO_UNIT_AUDIO_FIXED_BUFFER_SIZE (512)

typedef struct _AgsFxAudioUnitAudio AgsFxAudioUnitAudio;
typedef struct _AgsFxAudioUnitAudioScopeData AgsFxAudioUnitAudioScopeData;
typedef struct _AgsFxAudioUnitAudioChannelData AgsFxAudioUnitAudioChannelData;
typedef struct _AgsFxAudioUnitAudioInputData AgsFxAudioUnitAudioInputData;
typedef struct _AgsFxAudioUnitAudioClass AgsFxAudioUnitAudioClass;

typedef enum{
  AGS_FX_AUDIO_UNIT_AUDIO_MONO            = 1,
  //NOTE:JK: stereo and more channels require super-threaded channel, because of the dispatcher
  AGS_FX_AUDIO_UNIT_AUDIO_STEREO          = 1 <<  1,
  AGS_FX_AUDIO_UNIT_AUDIO_SURROUND_2_1    = 1 <<  3,
  AGS_FX_AUDIO_UNIT_AUDIO_SURROUND_5_1    = 1 <<  4,
  AGS_FX_AUDIO_UNIT_AUDIO_SURROUND_7_1    = 1 <<  5,
}AgsFxAudioUnitAudioFlags;

struct _AgsFxAudioUnitAudio
{
  AgsFxNotationAudio fx_notation_audio;

  AgsFxAudioUnitAudioFlags flags;
    
  AgsAudioUnitPlugin *audio_unit_plugin;

  gpointer av_format;
  
  gpointer audio_engine;
  
  gpointer av_audio_unit;
  gpointer av_audio_sequencer;
  
  gpointer av_music_track;
  
  gboolean render_thread_running;
  _Atomic gint render_ref_count;
  
  GThread *render_thread;
  
  _Atomic gboolean pre_sync_wait;
  _Atomic gint active_iteration_count;
  
  GMutex pre_sync_mutex;
  GCond pre_sync_cond;
  
  _Atomic gboolean prepare_iteration_wait;

  _Atomic GList *iterate_data;
  _Atomic gpointer active_iterate_data;
  
  GMutex prepare_iteration_mutex;
  GCond prepare_iteration_cond;

  _Atomic gboolean completed_iteration_wait;
  _Atomic gboolean completed_iteration_done;
  
  GMutex completed_iteration_mutex;
  GCond completed_iteration_cond;
  
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

  gboolean running;

  _Atomic guint active_audio_channels;
  _Atomic guint completed_audio_channels;
  
  guint audio_channels;

  gfloat *output;
  guint output_buffer_size;
  
  gfloat *input;
  guint input_buffer_size;

  gpointer av_output;

  gpointer av_input;

  AgsVolumeUtil *volume_util;

  _Atomic gboolean pre_sync_wait;
  _Atomic gint active_pre_sync_count;
  _Atomic gint completed_pre_sync_count;

  GMutex completed_pre_sync_mutex;
  GCond completed_pre_sync_cond;

  _Atomic gboolean audio_signal_wait;
  _Atomic gint active_audio_signal_count;
  _Atomic gint completed_audio_signal_count;

  GMutex completed_audio_signal_mutex;
  GCond completed_audio_signal_cond;

  GHashTable *active_audio_signal;
  
  GHashTable *written_audio_signal;

  _Atomic gboolean render_done;
  _Atomic gboolean render_wait;

  GMutex render_mutex;
  GCond render_cond;
  
  AgsFxAudioUnitAudioChannelData **channel_data;
};

struct _AgsFxAudioUnitAudioChannelData
{
  GRecMutex strct_mutex;
  
  gpointer parent;

  gfloat *output;
  guint output_buffer_size;
  
  gfloat *input;
  guint input_buffer_size;

  gpointer av_output;

  gpointer av_input;

  gpointer audio_buffer_list;
  
  AgsVolumeUtil *volume_util;

  guint event_count;

  AgsFxAudioUnitAudioInputData* input_data[AGS_SEQUENCER_MAX_MIDI_KEYS];
};

struct _AgsFxAudioUnitAudioInputData
{
  GRecMutex strct_mutex;
  
  gpointer parent;

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
gboolean ags_fx_audio_unit_audio_test_flags(AgsFxAudioUnitAudio *fx_audio_unit_audio, AgsFxAudioUnitAudioFlags flags);
void ags_fx_audio_unit_audio_set_flags(AgsFxAudioUnitAudio *fx_audio_unit_audio, AgsFxAudioUnitAudioFlags flags);
void ags_fx_audio_unit_audio_unset_flags(AgsFxAudioUnitAudio *fx_audio_unit_audio, AgsFxAudioUnitAudioFlags flags);

/* load/unload */
void ags_fx_audio_unit_audio_load_plugin(AgsFxAudioUnitAudio *fx_audio_unit_audio);
void ags_fx_audio_unit_audio_load_port(AgsFxAudioUnitAudio *fx_audio_unit_audio);

/* render thread */
void ags_fx_audio_unit_audio_start_render_thread(AgsFxAudioUnitAudio *fx_audio_unit_audio);
void ags_fx_audio_unit_audio_stop_render_thread(AgsFxAudioUnitAudio *fx_audio_unit_audio);

void ags_fx_audio_unit_audio_render_thread_iteration(AgsFxAudioUnitAudio *fx_audio_unit_audio,
						     AgsFxAudioUnitAudioChannelData *channel_data,
						     AgsAudioSignal *audio_signal,
						     guint pad,
						     guint audio_channel);

/* instantiate */
AgsFxAudioUnitAudio* ags_fx_audio_unit_audio_new(AgsAudio *audio);

G_END_DECLS

#endif /*__AGS_FX_AUDIO_UNIT_AUDIO_H__*/
