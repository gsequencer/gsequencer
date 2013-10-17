/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2013 Joël Krähemann
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

#include <ags/file/ags_embedded_audio.h>

void ags_embedded_audio_class_init(AgsEmbeddedAudioClass *embedded_audio);
void ags_embedded_audio_init(AgsEmbeddedAudio *embedded_audio);
void ags_embedded_audio_set_property(GObject *gobject,
				     guint prop_id,
				     const GValue *value,
				     GParamSpec *param_spec);
void ags_embedded_audio_get_property(GObject *gobject,
				     guint prop_id,
				     GValue *value,
				     GParamSpec *param_spec);
void ags_embedded_audio_finalize(GObject *gobject);

enum{
  PROP_0,
  PROP_DATA,
};

static gpointer ags_embedded_audio_parent_class = NULL;

GType
ags_embedded_audio_get_type()
{
  static GType ags_type_embedded_audio = 0;

  if(!ags_type_embedded_audio){
    static const GTypeInfo ags_embedded_audio_info = {
      sizeof (AgsEmbeddedAudioClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_embedded_audio_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsEmbeddedAudio),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_embedded_audio_init,
    };

    ags_type_embedded_audio = g_type_register_static(G_TYPE_OBJECT,
						     "AgsEmbeddedAudio\0",
						     &ags_embedded_audio_info,
						     0);
  }

  return (ags_type_embedded_audio);
}

void
ags_embedded_audio_class_init(AgsEmbeddedAudioClass *embedded_audio)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_embedded_audio_parent_class = g_type_class_peek_parent(embedded_audio);

  /* GObjectClass */
  gobject = (GObjectClass *) embedded_audio;

  gobject->set_property = ags_embedded_audio_set_property;
  gobject->get_property = ags_embedded_audio_get_property;

  gobject->finalize = ags_embedded_audio_finalize;

  /* properties */
  param_spec = g_param_spec_pointer("data\0",
				    "the data\0",
				    "The data to locate the file\0",
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DATA,
				  param_spec);
}

void
ags_embedded_audio_init(AgsEmbeddedAudio *embedded_audio)
{
  embedded_audio->data = NULL;
}

void
ags_embedded_audio_set_property(GObject *gobject,
				guint prop_id,
				const GValue *value,
				GParamSpec *param_spec)
{
  AgsEmbeddedAudio *embedded_audio;

  embedded_audio = AGS_EMBEDDED_AUDIO(gobject);
  
  switch(prop_id){
  case PROP_DATA:
    {
      gpointer data;

      data = (gpointer) g_value_get_pointer(value);

      if(embedded_audio->data != NULL)
	g_free(embedded_audio->data);

      embedded_audio->data = data;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_embedded_audio_get_property(GObject *gobject,
				guint prop_id,
				GValue *value,
				GParamSpec *param_spec)
{
  AgsEmbeddedAudio *embedded_audio;

  embedded_audio = AGS_EMBEDDED_AUDIO(gobject);
  
  switch(prop_id){
  case PROP_DATA:
    {
      g_value_set_pointer(value, embedded_audio->data);
    }
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_embedded_audio_finalize(GObject *gobject)
{
}

AgsEmbeddedAudio*
ags_embedded_audio_new()
{
  AgsEmbeddedAudio *embedded_audio;

  embedded_audio = (AgsEmbeddedAudio *) g_object_new(AGS_TYPE_EMBEDDED_AUDIO,
						     NULL);

  return(embedded_audio);
}
