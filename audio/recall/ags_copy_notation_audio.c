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

#include <ags/audio/recall/ags_copy_notation_audio.h>

#include <ags/audio/recall/ags_copy_notation_audio_run.h>
#include <ags/audio/recall/ags_copy_notation_channel.h>
#include <ags/audio/recall/ags_copy_notation_channel_run.h>

void ags_copy_notation_audio_class_init(AgsCopyNotationAudioClass *copy_notation_audio);
void ags_copy_notation_audio_init(AgsCopyNotationAudio *copy_notation_audio);
void ags_copy_notation_audio_set_property(GObject *gobject,
					  guint prop_id,
					  const GValue *value,
					  GParamSpec *param_spec);
void ags_copy_notation_audio_get_property(GObject *gobject,
					  guint prop_id,
					  GValue *value,
					  GParamSpec *param_spec);
void ags_copy_notation_audio_finalize(GObject *gobject);

enum{
  PROP_0,
  PROP_NOTATION,
};

static gpointer ags_copy_notation_audio_parent_class = NULL;

GType
ags_copy_notation_audio_get_type()
{
  static GType ags_type_copy_notation_audio = 0;

  if(!ags_type_copy_notation_audio){
    static const GTypeInfo ags_copy_notation_audio_info = {
      sizeof (AgsCopyNotationAudioClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_copy_notation_audio_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsCopyNotationAudio),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_copy_notation_audio_init,
    };

    ags_type_copy_notation_audio = g_type_register_static(AGS_TYPE_RECALL_AUDIO,
							  "AgsCopyNotationAudio\0",
							  &ags_copy_notation_audio_info,
							  0);
  }

  return(ags_type_copy_notation_audio);
}

void
ags_copy_notation_audio_class_init(AgsCopyNotationAudioClass *copy_notation_audio)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_copy_notation_audio_parent_class = g_type_class_peek_parent(copy_notation_audio);

  /* GObjectClass */
  gobject = (GObjectClass *) copy_notation_audio;

  gobject->set_property = ags_copy_notation_audio_set_property;
  gobject->get_property = ags_copy_notation_audio_get_property;

  gobject->finalize = ags_copy_notation_audio_finalize;

  /* properties */
  param_spec = g_param_spec_object("notation\0",
				   "the assigned notation\0",
				   "The AgsNotation it is assigned to\0",
				   AGS_TYPE_NOTATION,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_NOTATION,
				  param_spec);
}

void
ags_copy_notation_audio_init(AgsCopyNotationAudio *copy_notation_audio)
{
  copy_notation_audio->notation = NULL;
}

void
ags_copy_notation_audio_set_property(GObject *gobject,
				    guint prop_id,
				    const GValue *value,
				    GParamSpec *param_spec)
{
  AgsCopyNotationAudio *copy_notation_audio;

  copy_notation_audio = AGS_COPY_NOTATION_AUDIO(gobject);

  switch(prop_id){
  case PROP_NOTATION:
    {
      AgsNotation *notation;

      notation = (AgsNotation *) g_value_get_object(value);

      if(copy_notation_audio->notation != notation)
	return;

      if(copy_notation_audio->notation != NULL)
	g_object_unref(copy_notation_audio->notation);

      if(notation != NULL)
	g_object_ref(notation);

      copy_notation_audio->notation = notation;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_copy_notation_audio_get_property(GObject *gobject,
				    guint prop_id,
				    GValue *value,
				    GParamSpec *param_spec)
{
  AgsCopyNotationAudio *copy_notation;
  
  copy_notation_audio = AGS_COPY_NOTATION_AUDIO(gobject);

  switch(prop_id){
  case PROP_NOTATION:
    {
      g_value_set_object(value, copy_notation_audio->notation);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_copy_notation_audio_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_copy_notation_audio_parent_class)->finalize(gobject);
}

AgsCopyNotationAudio*
ags_copy_notation_audio_new()
{
  AgsCopyNotationAudio *copy_notation_audio;

  copy_notation_audio = (AgsCopyNotationAudio *) g_object_new(AGS_TYPE_COPY_NOTATION_AUDIO,
							      NULL);
  
  return(copy_notation_audio);
}
