/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <ags/audio/recall/ags_count_beats_audio.h>

#include <ags/audio/recall/ags_count_beats_audio_run.h>

void ags_count_beats_audio_class_init(AgsCountBeatsAudioClass *count_beats_audio);
void ags_count_beats_audio_init(AgsCountBeatsAudio *count_beats_audio);
void ags_count_beats_audio_set_property(GObject *gobject,
					guint prop_id,
					const GValue *value,
					GParamSpec *param_spec);
void ags_count_beats_audio_get_property(GObject *gobject,
					guint prop_id,
					GValue *value,
					GParamSpec *param_spec);
void ags_count_beats_audio_finalize(GObject *gobject);

enum{
  PROP_0,
  PROP_LOOP,
  PROP_NOTATION_LOOP_START,
  PROP_NOTATION_LOOP_END,
  PROP_SEQUENCER_LOOP_START,
  PROP_SEQUENCER_LOOP_END,
};

static gpointer ags_count_beats_audio_parent_class = NULL;

GType
ags_count_beats_audio_get_type()
{
  static GType ags_type_count_beats_audio = 0;

  if(!ags_type_count_beats_audio){
    static const GTypeInfo ags_count_beats_audio_info = {
      sizeof (AgsCountBeatsAudioClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_count_beats_audio_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsCountBeatsAudio),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_count_beats_audio_init,
    };
    
    ags_type_count_beats_audio = g_type_register_static(AGS_TYPE_RECALL_AUDIO,
							"AgsCountBeatsAudio",
							&ags_count_beats_audio_info,
							0);
  }

  return(ags_type_count_beats_audio);
}

void
ags_count_beats_audio_class_init(AgsCountBeatsAudioClass *count_beats_audio)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_count_beats_audio_parent_class = g_type_class_peek_parent(count_beats_audio);

  gobject = (GObjectClass *) count_beats_audio;

  gobject->set_property = ags_count_beats_audio_set_property;
  gobject->get_property = ags_count_beats_audio_get_property;

  gobject->finalize = ags_count_beats_audio_finalize;

  /* properties */
  param_spec = g_param_spec_boolean("loop",
				    "loop playing",
				    "Play in a endless loop",
				    FALSE,
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_LOOP,
				  param_spec);

  param_spec = g_param_spec_uint("notation_loop_start",
				 "start beat of notation loop",
				 "The start beat of the notation loop",
				 0,
				 65535,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_NOTATION_LOOP_START,
				  param_spec);

  param_spec = g_param_spec_uint("notation_loop_end",
				 "end beat of notation loop",
				 "The end beat of the notation loop",
				 0,
				 65535,
				 64,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_NOTATION_LOOP_END,
				  param_spec);

  param_spec = g_param_spec_uint("sequencer_loop_start",
				 "start beat of loop",
				 "The start beat of the sequencer loop",
				 0,
				 65535,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SEQUENCER_LOOP_START,
				  param_spec);

  param_spec = g_param_spec_uint("sequencer_loop_end",
				 "end beat of sequencer loop",
				 "The end beat of the sequencer loop",
				 16,
				 65535,
				 16,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SEQUENCER_LOOP_END,
				  param_spec);
}

void
ags_count_beats_audio_init(AgsCountBeatsAudio *count_beats_audio)
{
  count_beats_audio->loop = FALSE;

  count_beats_audio->notation_loop_start = 0;
  count_beats_audio->notation_loop_end = 64;

  count_beats_audio->sequencer_loop_start = 0;
  count_beats_audio->sequencer_loop_end = 16;
}

void
ags_count_beats_audio_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec)
{
  AgsCountBeatsAudio *count_beats_audio;

  count_beats_audio = AGS_COUNT_BEATS_AUDIO(gobject);

  switch(prop_id){
  case PROP_LOOP:
    {
      gboolean loop;

      loop = g_value_get_boolean(value);

      count_beats_audio->loop = loop;
    }
    break;
  case PROP_NOTATION_LOOP_START:
    {
      guint loop_start;

      loop_start = g_value_get_uint(value);

      count_beats_audio->notation_loop_start = loop_start;
    }
    break;
  case PROP_NOTATION_LOOP_END:
    {
      guint loop_end;

      loop_end = g_value_get_uint(value);

      count_beats_audio->notation_loop_end = loop_end;
    }
    break;
  case PROP_SEQUENCER_LOOP_START:
    {
      guint loop_start;

      loop_start = g_value_get_uint(value);

      count_beats_audio->sequencer_loop_start = loop_start;
    }
    break;
  case PROP_SEQUENCER_LOOP_END:
    {
      guint loop_end;

      loop_end = g_value_get_uint(value);

      count_beats_audio->sequencer_loop_end = loop_end;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_count_beats_audio_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec)
{
  AgsCountBeatsAudio *count_beats;
  
  count_beats = AGS_COUNT_BEATS_AUDIO(gobject);

  switch(prop_id){
  case PROP_LOOP:
    {
      g_value_set_boolean(value, count_beats->loop);
    }
    break;
  case PROP_NOTATION_LOOP_START:
    {
      g_value_set_uint(value, count_beats->notation_loop_start);
    }
    break;
  case PROP_NOTATION_LOOP_END:
    {
      g_value_set_uint(value, count_beats->notation_loop_end);
    }
    break;
  case PROP_SEQUENCER_LOOP_START:
    {
      g_value_set_uint(value, count_beats->sequencer_loop_start);
    }
    break;
  case PROP_SEQUENCER_LOOP_END:
    {
      g_value_set_uint(value, count_beats->sequencer_loop_end);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_count_beats_audio_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_count_beats_audio_parent_class)->finalize(gobject);
}

AgsCountBeatsAudio*
ags_count_beats_audio_new(gboolean loop)
{
  AgsCountBeatsAudio *count_beats_audio;

  count_beats_audio = (AgsCountBeatsAudio *) g_object_new(AGS_TYPE_COUNT_BEATS_AUDIO,
							  "loop", loop,
							  NULL);
  
  return(count_beats_audio);
}
