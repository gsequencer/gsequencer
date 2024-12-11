/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2024 Joël Krähemann
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

#include <lv2/atom/atom.h>
#include <lv2/atom/forge.h>
#include <lv2/atom/util.h>

#include <stdint.h>
#include <string.h>

#include <ags/plugin/ags_lv2_plugin.h>

#include <ags/audio/ags_port.h>

#include <ags/audio/fx/ags_fx_notation_audio.h>

G_BEGIN_DECLS

#define AGS_TYPE_FX_LV2_AUDIO                (ags_fx_lv2_audio_get_type())
#define AGS_FX_LV2_AUDIO(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_FX_LV2_AUDIO, AgsFxLv2Audio))
#define AGS_FX_LV2_AUDIO_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_FX_LV2_AUDIO, AgsFxLv2AudioClass))
#define AGS_IS_FX_LV2_AUDIO(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_FX_LV2_AUDIO))
#define AGS_IS_FX_LV2_AUDIO_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_FX_LV2_AUDIO))
#define AGS_FX_LV2_AUDIO_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_FX_LV2_AUDIO, AgsFxLv2AudioClass))

#define AGS_FX_LV2_AUDIO_SCOPE_DATA(ptr) ((AgsFxLv2AudioScopeData *) (ptr))
#define AGS_FX_LV2_AUDIO_SCOPE_DATA_GET_STRCT_MUTEX(ptr) (&(((AgsFxLv2AudioScopeData *)(ptr))->strct_mutex))

#define AGS_FX_LV2_AUDIO_CHANNEL_DATA(ptr) ((AgsFxLv2AudioChannelData *) (ptr))
#define AGS_FX_LV2_AUDIO_CHANNEL_DATA_GET_STRCT_MUTEX(ptr) (&(((AgsFxLv2AudioChannelData *)(ptr))->strct_mutex))

#define AGS_FX_LV2_AUDIO_INPUT_DATA(ptr) ((AgsFxLv2AudioInputData *) (ptr))
#define AGS_FX_LV2_AUDIO_INPUT_DATA_GET_STRCT_MUTEX(ptr) (&(((AgsFxLv2AudioInputData *)(ptr))->strct_mutex))

#define AGS_FX_LV2_AUDIO_DEFAULT_MIDI_LENGHT (4096)

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

  AgsFxLv2AudioFlags flags;
  
  guint output_port_count;
  guint *output_port;

  guint input_port_count;
  guint *input_port;

  gboolean has_midiin_event_port;
  guint midiin_event_port;
  
  gboolean has_midiin_atom_port;
  guint midiin_atom_port;
  
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
  GRecMutex strct_mutex;
  
  gpointer parent;
  
  guint audio_channels;
  
  AgsFxLv2AudioChannelData **channel_data;
};

struct _AgsFxLv2AudioChannelData
{
  GRecMutex strct_mutex;
  
  gpointer parent;

  guint event_count;
  
  float *output;
  float *input;

  gpointer midiin_event_port;
  gpointer midiout_event_port;
  
  LV2_Atom_Forge forge;
  LV2_Atom_Forge_Frame frame;

  uint8_t *forge_buffer;
  uint32_t forge_buffer_size;
  
  LV2_URID_Map *urid_map;
  
  LV2_URID atom_Blank;
  LV2_URID atom_Object;
  LV2_URID atom_Sequence;
  LV2_URID midi_MidiEvent;
  
  LV2_URID atom_Float;
  LV2_URID atom_Int;
  LV2_URID atom_Long;
  LV2_URID time_Position;
  LV2_URID time_bar;
  LV2_URID time_barBeat;
  LV2_URID time_beatUnit;
  LV2_URID time_beatsPerBar;
  LV2_URID time_beatsPerMinute;
  LV2_URID time_speed;
  
  uint8_t *midiin_atom_port;
  uint32_t midiin_atom_port_size;
  
  uint8_t *midiout_atom_port;
  uint32_t midiout_atom_port_size;
  
  LV2_Handle *lv2_handle;

  AgsFxLv2AudioInputData* input_data[AGS_SEQUENCER_MAX_MIDI_KEYS];
};

struct _AgsFxLv2AudioInputData
{
  GRecMutex strct_mutex;
  
  gpointer parent;

  float *output;
  float *input;

  gpointer midiin_event_port;
  gpointer midiout_event_port;
  
  gpointer midiin_atom_port;
  gpointer midiout_atom_port;

  LV2_Handle *lv2_handle;
  
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
gboolean ags_fx_lv2_audio_test_flags(AgsFxLv2Audio *fx_lv2_audio, AgsFxLv2AudioFlags flags);
void ags_fx_lv2_audio_set_flags(AgsFxLv2Audio *fx_lv2_audio, AgsFxLv2AudioFlags flags);
void ags_fx_lv2_audio_unset_flags(AgsFxLv2Audio *fx_lv2_audio, AgsFxLv2AudioFlags flags);

/* lv2 specific */
void ags_fx_lv2_audio_forge_midi_message(AgsFxLv2Audio *fx_lv2_audio,
					 AgsFxLv2AudioChannelData *channel_data,
					 uint32_t offset,
					 const uint8_t* const midi_buffer,
					 uint32_t midi_buffer_size);

/**
   A forge sink that writes to an atom .

   It is assumed that the handle points to an LV2_Atom large enough to store
   the forge output.  The forged result is in the body of the buffer atom.
*/
static LV2_Atom_Forge_Ref
ags_lv2_midiin_atom_sink(LV2_Atom_Forge_Sink_Handle handle, const void* buf, uint32_t size)
{
  AgsFxLv2AudioChannelData *channel_data = (AgsFxLv2AudioChannelData *) handle;
  LV2_Atom *atom = (LV2_Atom *) (channel_data->midiin_atom_port);
  const uint32_t offset = lv2_atom_total_size(atom);
  memcpy((char*) channel_data->midiin_atom_port + offset, buf, size);
  atom->size += size;
  return offset;
}

/**
   Dereference counterpart to ags_lv2_midiin_atom_sink().
*/
static LV2_Atom*
ags_lv2_midiin_atom_sink_deref(LV2_Atom_Forge_Sink_Handle handle, LV2_Atom_Forge_Ref ref)
{
  AgsFxLv2AudioChannelData *channel_data = (AgsFxLv2AudioChannelData *) handle;
  
  return((LV2_Atom *)((char *) channel_data->midiin_atom_port + ref));
}

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
