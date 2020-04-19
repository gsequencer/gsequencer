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

#include <ladspa.h>
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

typedef struct _AgsFxLv2Audio AgsFxLv2Audio;
typedef struct _AgsFxLv2AudioClass AgsFxLv2AudioClass;

typedef enum{
  AGS_FX_LV2_AUDIO_INSTRUMENT          = 1,
  AGS_FX_LV2_AUDIO_LIVE_INSTRUMENT     = 1 <<  1,
}AgsFxLv2AudioFlags;

struct _AgsFxLv2Audio
{
  AgsFxNotationAudio fx_notation_audio;

  guint flags;
  
  guint input_lines;
  guint output_lines;

  guint *input_port;
  guint *output_port;

  guint bank;
  guint program;

  float **input;
  float **output;
  
  guint event_count;
  snd_seq_event_t* event_buffer[128];

  guint key_on[128];
  
  LV2_Handle *lv2_handle;

  AgsLv2Plugin *lv2_plugin;

  AgsPort **lv2_port;
};

struct _AgsFxLv2AudioClass
{
  AgsFxNotationAudioClass fx_notation_audio;
  
  void (*change_program)(AgsLv2Plugin *lv2_plugin,
			 guint bank_index,
			 guint program_index);
};

GType ags_fx_lv2_audio_get_type();

gboolean ags_fx_lv2_audio_test_flags(AgsFxLv2Audio *fx_lv2_audio, guint flags);
void ags_fx_lv2_audio_set_flags(AgsFxLv2Audio *fx_lv2_audio, guint flags);
void ags_fx_lv2_audio_unset_flags(AgsFxLv2Audio *fx_lv2_audio, guint flags);

void ags_fx_lv2_audio_load_plugin(AgsFxLv2Audio *fx_lv2_audio);
void ags_fx_lv2_audio_load_port(AgsFxLv2Audio *fx_lv2_audio);

void ags_fx_lv2_audio_unload_plugin(AgsFxLv2Audio *fx_lv2_audio);
void ags_fx_lv2_audio_unload_port(AgsFxLv2Audio *fx_lv2_audio);

void ags_fx_lv2_audio_change_program(AgsLv2Plugin *lv2_plugin,
				     guint bank_index,
				     guint program_index);

AgsFxLv2Audio* ags_fx_lv2_audio_new(AgsAudio *audio);

G_END_DECLS

#endif /*__AGS_FX_LV2_AUDIO_H__*/
