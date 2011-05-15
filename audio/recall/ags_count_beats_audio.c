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
  PROP_LENGTH,
  PROP_LOOP,
  PROP_STREAM_LENGTH,
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
							"AgsCountBeatsAudio\0",
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
  param_spec = g_param_spec_uint("length\0",
				 "length in beats\0",
				 "The length of the stream in beats\0",
				 1,
				 128,
				 16,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_LENGTH,
				  param_spec);

  param_spec = g_param_spec_boolean("loop\0",
				    "loop playing\0",
				    "Play in a endless loop\0",
				    FALSE,
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_LOOP,
				  param_spec);

  param_spec = g_param_spec_uint("stream_length\0",
				 "stream length in buffers\0",
				 "The length of the stream in buffer count\0",
				 1,
				 65535,
				 1,
				 G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_STREAM_LENGTH,
				  param_spec);
}

void
ags_count_beats_audio_init(AgsCountBeatsAudio *count_beats_audio)
{
  count_beats_audio->length = 0;
  count_beats_audio->loop = FALSE;

  count_beats_audio->stream_length = 0;
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
  case PROP_LENGTH:
    {
      guint length;

      length = g_value_get_uint(value);

      count_beats_audio->length = length;

      /*
       * FIXME:JK: calculate stream_length with length
       */
    }
    break;
  case PROP_LOOP:
    {
      gboolean loop;

      loop = g_value_get_boolean(value);

      count_beats_audio->loop = loop;
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
  case PROP_LENGTH:
    {
      g_value_set_uint(value, count_beats->length);
    }
    break;
  case PROP_LOOP:
    {
      g_value_set_boolean(value, count_beats->loop);
    }
    break;
  case PROP_STREAM_LENGTH:
    {
      g_value_set_uint(value, count_beats->stream_length);
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
ags_count_beats_audio_new(double bpm,
			  guint length,
			  gboolean loop)
{
  AgsCountBeatsAudio *count_beats_audio;

  count_beats_audio = (AgsCountBeatsAudio *) g_object_new(AGS_TYPE_COUNT_BEATS_AUDIO,
							  "bpm\0", bpm,
							  "length\0", length,
							  "loop\0", loop,
							  NULL);
  
  return(count_beats_audio);
}
