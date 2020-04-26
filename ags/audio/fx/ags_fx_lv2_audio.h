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

#ifndef __AGS_FX_LV2_AUDIO_H__
#define __AGS_FX_LV2_AUDIO_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <alsa/seq_event.h>

#include <lv2.h>

#include <ags/plugin/ags_lv2_plugin.h>

#include <ags/audio/ags_port.h>

#include <ags/audio/fx/ags_fx_notation_audio.h>

G_BEGIN_DECLS

#define AGS_TYPE_FX_LV2_AUDIO                (ags_fx_lv2_audio_get_type())
#define AGS_FX_LV2_AUDIO(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_FX_LV2_AUDIO, AgsFxLv2Audio))
#define AGS_FX_LV2_AUDIO_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_FX_LV2_AUDIO, AgsFxLv2Audio))
#define AGS_IS_FX_LV2_AUDIO(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_FX_LV2_AUDIO))
#define AGS_IS_FX_LV2_AUDIO_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_FX_LV2_AUDIO))
#define AGS_FX_LV2_AUDIO_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_FX_LV2_AUDIO, AgsFxLv2AudioClass))

#define AGS_FX_LV2_AUDIO_SCOPE_DATA(ptr) ((AgsFxLv2AudioScopeData *) (ptr))
#define AGS_FX_LV2_AUDIO_CHANNEL_DATA(ptr) ((AgsFxLv2AudioChannelData *) (ptr))
#define AGS_FX_LV2_AUDIO_INPUT_DATA(ptr) ((AgsFxLv2AudioInputData *) (ptr))

typedef struct _AgsFxLv2Audio AgsFxLv2Audio;
typedef struct _AgsFxLv2AudioScopeData AgsFxLv2AudioScopeData;
typedef struct _AgsFxLv2AudioChannelData AgsFxLv2AudioChannelData;
typedef struct _AgsFxLv2AudioInputData AgsFxLv2AudioInputData;
typedef struct _AgsFxLv2AudioClass AgsFxLv2AudioClass;

typedef enum{
  AGS_FX_LV2_AUDIO_LIVE_INSTRUMENT     = 1,
}AgsFxLv2AudioFlags;

struct _AgsFxLv2Audio
{
  AgsFxNotationAudio fx_notation_audio;

  guint flags;
  
  guint output_port_count;
  guint *output_port;

  guint input_port_count;
  guint *input_port;

  guint bank;
  guint program;

  AgsFxLv2AudioScopeData* scope_data[AGS_SOUND_SCOPE_LAST];

  AgsLv2Plugin *lv2_plugin;

  AgsPort **lv2_port;
};

struct _AgsFxLv2AudioClass
{
  AgsFxNotationAudioClass fx_notation_audio;
};

struct _AgsFxLv2AudioScopeData
{
  gpointer parent;
  
  guint audio_channels;
  
  AgsFxLv2AudioChannelData **channel_data;
};

struct _AgsFxLv2AudioChannelData
{
  gpointer parent;

  guint event_count;
  
  float *output;
  float *input;
  
  LV2_Handle lv2_handle;

  AgsFxLv2AudioInputData* input_data[AGS_SEQUENCER_MAX_MIDI_KEYS];
};

struct _AgsFxLv2AudioInputData
{
  gpointer parent;

  float *output;
  float *input;

  LV2_Handle lv2_handle;
  
  snd_seq_event_t *event_buffer;
  guint key_on;
};

GType ags_fx_lv2_audio_get_type();

/* runtime */
AgsFxLv2AudioScopeData* ags_fx_lv2_audio_scope_data_alloc();
void ags_fx_lv2_audio_scope_data_free(AgsFxLv2AudioScopeData *scope_data);

AgsFxLv2AudioChannelData* ags_fx_lv2_audio_channel_data_alloc();
void ags_fx_lv2_audio_channel_data_free(AgsFxLv2AudioChannelData *channel_data);

AgsFxLv2AudioInputData* ags_fx_lv2_audio_input_data_alloc();
void ags_fx_lv2_audio_input_data_free(AgsFxLv2AudioInputData *input_data);

/* flags */
gboolean ags_fx_lv2_audio_test_flags(AgsFxLv2Audio *fx_lv2_audio, guint flags);
void ags_fx_lv2_audio_set_flags(AgsFxLv2Audio *fx_lv2_audio, guint flags);
void ags_fx_lv2_audio_unset_flags(AgsFxLv2Audio *fx_lv2_audio, guint flags);

/* load/unload */
void ags_fx_lv2_audio_load_plugin(AgsFxLv2Audio *fx_lv2_audio);
void ags_fx_lv2_audio_load_port(AgsFxLv2Audio *fx_lv2_audio);

/* plugin */
void ags_fx_lv2_audio_change_program(AgsFxLv2Audio *fx_lv2_audio,
				     guint bank_index,
				     guint program_index);

/* instantiate */
AgsFxLv2Audio* ags_fx_lv2_audio_new(AgsAudio *audio);

G_END_DECLS

#endif /*__AGS_FX_LV2_AUDIO_H__*/
