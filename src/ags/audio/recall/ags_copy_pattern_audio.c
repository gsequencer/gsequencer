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

#include <ags/audio/recall/ags_copy_pattern_audio.h>

#include <ags/audio/recall/ags_copy_pattern_audio_run.h>
#include <ags/audio/recall/ags_copy_pattern_channel.h>
#include <ags/audio/recall/ags_copy_pattern_channel_run.h>

void ags_copy_pattern_audio_class_init(AgsCopyPatternAudioClass *copy_pattern_audio);
void ags_copy_pattern_audio_init(AgsCopyPatternAudio *copy_pattern_audio);
void ags_copy_pattern_audio_set_property(GObject *gobject,
					 guint prop_id,
					 const GValue *value,
					 GParamSpec *param_spec);
void ags_copy_pattern_audio_get_property(GObject *gobject,
					 guint prop_id,
					 GValue *value,
					 GParamSpec *param_spec);
void ags_copy_pattern_audio_finalize(GObject *gobject);

enum{
  PROP_0,
  PROP_BANK_INDEX_0,
  PROP_BANK_INDEX_1,
};

static gpointer ags_copy_pattern_audio_parent_class = NULL;

GType
ags_copy_pattern_audio_get_type()
{
  static GType ags_type_copy_pattern_audio = 0;

  if(!ags_type_copy_pattern_audio){
    static const GTypeInfo ags_copy_pattern_audio_info = {
      sizeof (AgsCopyPatternAudioClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_copy_pattern_audio_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsCopyPatternAudio),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_copy_pattern_audio_init,
    };

    ags_type_copy_pattern_audio = g_type_register_static(AGS_TYPE_RECALL_AUDIO,
							 "AgsCopyPatternAudio",
							 &ags_copy_pattern_audio_info,
							 0);
  }

  return(ags_type_copy_pattern_audio);
}

void
ags_copy_pattern_audio_class_init(AgsCopyPatternAudioClass *copy_pattern_audio)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_copy_pattern_audio_parent_class = g_type_class_peek_parent(copy_pattern_audio);

  /* GObjectClass */
  gobject = (GObjectClass *) copy_pattern_audio;

  gobject->set_property = ags_copy_pattern_audio_set_property;
  gobject->get_property = ags_copy_pattern_audio_get_property;

  gobject->finalize = ags_copy_pattern_audio_finalize;

  /* properties */
  param_spec = g_param_spec_uint("bank_index_0",
				 "current bank index 0",
				 "The current bank index 0 of the AgsPattern",
				 0,
				 256,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BANK_INDEX_0,
				  param_spec);

  param_spec = g_param_spec_uint("bank_index_1",
				 "current bank index 1",
				 "The current bank index 1 of the AgsPattern",
				 0,
				 256,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BANK_INDEX_1,
				  param_spec);
}

void
ags_copy_pattern_audio_init(AgsCopyPatternAudio *copy_pattern_audio)
{
  AGS_RECALL(copy_pattern_audio)->flags |= AGS_RECALL_SEQUENCER;

  copy_pattern_audio->i = 0;
  copy_pattern_audio->j = 0;
}

void
ags_copy_pattern_audio_set_property(GObject *gobject,
				    guint prop_id,
				    const GValue *value,
				    GParamSpec *param_spec)
{
  AgsCopyPatternAudio *copy_pattern_audio;

  copy_pattern_audio = AGS_COPY_PATTERN_AUDIO(gobject);

  switch(prop_id){
  case PROP_BANK_INDEX_0:
    {
      guint i;

      i = g_value_get_uint(value);

      copy_pattern_audio->i = i;
    }
    break;
  case PROP_BANK_INDEX_1:
    {
      guint j;

      j = g_value_get_uint(value);

      copy_pattern_audio->j = j;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_copy_pattern_audio_get_property(GObject *gobject,
				    guint prop_id,
				    GValue *value,
				    GParamSpec *param_spec)
{
  AgsCopyPatternAudio *copy_pattern;
  
  copy_pattern = AGS_COPY_PATTERN_AUDIO(gobject);

  switch(prop_id){
  case PROP_BANK_INDEX_0:
    {
      g_value_set_uint(value, copy_pattern->i);
    }
    break;
  case PROP_BANK_INDEX_1:
    {
      g_value_set_uint(value, copy_pattern->j);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_copy_pattern_audio_finalize(GObject *gobject)
{
  AgsCopyPatternAudio *copy_pattern_audio;
  
  copy_pattern_audio = AGS_COPY_PATTERN_AUDIO(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_copy_pattern_audio_parent_class)->finalize(gobject);
}

AgsCopyPatternAudio*
ags_copy_pattern_audio_new(AgsDevout *devout,
			   gdouble tact,
			   guint i, guint j)
{
  AgsCopyPatternAudio *copy_pattern_audio;

  copy_pattern_audio = (AgsCopyPatternAudio *) g_object_new(AGS_TYPE_COPY_PATTERN_AUDIO,
							    "devout", devout,
							    "tact", tact,
							    "bank_index_0", i,
							    "bank_index_1", j,
							    NULL);

  return(copy_pattern_audio);
}
