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

#include <ags/audio/recall/ags_delay_audio.h>

#include <ags/audio/recall/ags_delay_audio_run.h>

void ags_delay_audio_class_init(AgsDelayAudioClass *delay_audio);
void ags_delay_audio_init(AgsDelayAudio *delay_audio);
void ags_delay_audio_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec);
void ags_delay_audio_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec);
void ags_delay_audio_finalize(GObject *gobject);

enum{
  PROP_0,
  PROP_DELAY,
};

static gpointer ags_delay_audio_parent_class = NULL;

GType
ags_delay_audio_get_type()
{
  static GType ags_type_delay_audio = 0;

  if(!ags_type_delay_audio){
    static const GTypeInfo ags_delay_audio_info = {
      sizeof (AgsDelayAudioClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_delay_audio_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio */
      sizeof (AgsDelayAudio),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_delay_audio_init,
    };

    ags_type_delay_audio = g_type_register_static(AGS_TYPE_RECALL_AUDIO,
						  "AgsDelayAudio\0",
						  &ags_delay_audio_info,
						  0);
  }

  return(ags_type_delay_audio);
}

void
ags_delay_audio_class_init(AgsDelayAudioClass *delay_audio)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_delay_audio_parent_class = g_type_class_peek_parent(delay_audio);

  /* GObjectClass */
  gobject = (GObjectClass *) delay_audio;

  gobject->set_property = ags_delay_audio_set_property;
  gobject->get_property = ags_delay_audio_get_property;

  gobject->finalize = ags_delay_audio_finalize;

  /* properties */
  param_spec = g_param_spec_gtype("delay\0",
				  "delay for timeing\0",
				  "The delay whenever a tic occures\0",
				   G_TYPE_UINT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DELAY,
				  param_spec);

}

void
ags_delay_audio_init(AgsDelayAudio *delay_audio)
{
  delay_audio->delay = 0;
}

void
ags_delay_audio_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec)
{
  AgsDelayAudio *delay_audio;

  delay_audio = AGS_DELAY_AUDIO(gobject);

  switch(prop_id){
  case PROP_DELAY:
    {
      guint delay;

      delay_audio->delay = (guint) g_value_get_uint(value);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_delay_audio_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec)
{
  AgsDelayAudio *delay_audio;

  delay_audio = AGS_DELAY_AUDIO(gobject);

  switch(prop_id){
  case PROP_DELAY:
    {
      g_value_set_uint(value, delay_audio->delay);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_delay_audio_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_delay_audio_parent_class)->finalize(gobject);
}

AgsDelayAudio*
ags_delay_audio_new(guint delay)
{
  AgsDelayAudio *delay_audio;

  delay_audio = (AgsDelayAudio *) g_object_new(AGS_TYPE_DELAY_AUDIO,
					       "delay\0", delay,
					       NULL);

  return(delay_audio);
}
