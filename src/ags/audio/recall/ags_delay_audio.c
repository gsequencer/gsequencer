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

#include <ags/object/ags_connectable.h>

void ags_delay_audio_class_init(AgsDelayAudioClass *delay_audio);
void ags_delay_audio_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_delay_audio_init(AgsDelayAudio *delay_audio);
void ags_delay_audio_connect(AgsConnectable *connectable);
void ags_delay_audio_disconnect(AgsConnectable *connectable);
void ags_delay_audio_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec);
void ags_delay_audio_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec);
void ags_delay_audio_finalize(GObject *gobject);

void ags_delay_audio_change_bpm(AgsTactable *tactable, gdouble bpm,
				AgsDelayAudio *delay_audio);
void ags_delay_audio_change_tact(AgsTactable *tactable, gdouble tact,
				 AgsDelayAudio *delay_audio);
void ags_delay_audio_change_duration(AgsTactable *tactable, gdouble duration,
				     AgsDelayAudio *delay_audio);

enum{
  PROP_0,
  PROP_TACTABLE,
  PROP_NOTATION_DELAY,
  PROP_SEQUENCER_DELAY,
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

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_delay_audio_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_delay_audio = g_type_register_static(AGS_TYPE_RECALL_AUDIO,
						  "AgsDelayAudio\0",
						  &ags_delay_audio_info,
						  0);

    g_type_add_interface_static(ags_type_delay_audio,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

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
  param_spec = g_param_spec_object("tactable\0",
				   "assigned tactable\0",
				   "The tactable it is assigned with\0",
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_TACTABLE,
				  param_spec);

  param_spec = g_param_spec_uint("notation_delay\0",
				 "notation delay for timeing\0",
				 "The notation delay whenever a tic occures\0",
				 0,
				 65535,
				 0,
				 G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_NOTATION_DELAY,
				  param_spec);

  param_spec = g_param_spec_uint("sequencer_delay\0",
				 "sequencer delay for timeing\0",
				 "The sequencer delay whenever a tic occures\0",
				 0,
				 256,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SEQUENCER_DELAY,
				  param_spec);
}

void
ags_delay_audio_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_delay_audio_connect;
  connectable->disconnect = ags_delay_audio_disconnect;
}

void
ags_delay_audio_init(AgsDelayAudio *delay_audio)
{
  delay_audio->bpm = 120.0;
  delay_audio->tact = 1.0 / 4.0;
  delay_audio->duration = 14.0;

  delay_audio->frames = 940 * 14;
  delay_audio->notation_delay = 1;
  delay_audio->sequencer_delay = 16;

  delay_audio->tactable = NULL;
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
  case PROP_NOTATION_DELAY:
    {
      guint delay;

      delay_audio->notation_delay = (guint) g_value_get_uint(value);
    }
    break;
  case PROP_SEQUENCER_DELAY:
    {
      guint delay;

      delay_audio->sequencer_delay = (guint) g_value_get_uint(value);
    }
    break;
  case PROP_TACTABLE:
    {
      AgsTactable *tactable;

      tactable = (AgsTactable *) g_value_get_object(value);

      if(tactable == delay_audio->tactable){
	return;
      }

      if(delay_audio->tactable != NULL){
	g_object_unref(G_OBJECT(delay_audio->tactable));
      }

      if(tactable != NULL){
	g_object_ref(G_OBJECT(tactable));
      }

      delay_audio->tactable = tactable;
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
  case PROP_NOTATION_DELAY:
    {
      g_value_set_uint(value, delay_audio->notation_delay);
    }
    break;
  case PROP_SEQUENCER_DELAY:
    {
      g_value_set_uint(value, delay_audio->sequencer_delay);
    }
    break;
  case PROP_TACTABLE:
    {
      g_value_set_object(value, delay_audio->tactable);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_delay_audio_connect(AgsConnectable *connectable)
{
  AgsDelayAudio *delay_audio;

  delay_audio = AGS_DELAY_AUDIO(connectable);

  if(delay_audio->tactable != NULL){
    delay_audio->change_bpm_handle =
      g_signal_connect(delay_audio->tactable, "change_bpm\0",
		       G_CALLBACK(ags_delay_audio_change_bpm), delay_audio);

    delay_audio->change_tact_handle = 
      g_signal_connect(delay_audio->tactable, "change_tact\0",
		       G_CALLBACK(ags_delay_audio_change_tact), delay_audio);

    delay_audio->change_duration_handle =
      g_signal_connect(delay_audio->tactable, "change_duration\0",
		       G_CALLBACK(ags_delay_audio_change_duration), delay_audio);
  }
}

void
ags_delay_audio_disconnect(AgsConnectable *connectable)
{
  AgsDelayAudio *delay_audio;

  delay_audio = AGS_DELAY_AUDIO(connectable);

  if(delay_audio->tactable != NULL){
    g_signal_handler_disconnect(delay_audio->tactable, delay_audio->change_bpm_handle);
    g_signal_handler_disconnect(delay_audio->tactable, delay_audio->change_tact_handle);
    g_signal_handler_disconnect(delay_audio->tactable, delay_audio->change_duration_handle);
  }
}

void
ags_delay_audio_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_delay_audio_parent_class)->finalize(gobject);
}

void
ags_delay_audio_change_bpm(AgsTactable *tactable, gdouble bpm,
			   AgsDelayAudio *delay_audio)
{
  AgsDevout *devout;

  devout = AGS_DEVOUT(AGS_RECALL_AUDIO(delay_audio)->audio->devout);

  delay_audio->frames = (((double) devout->frequency) *
			(60.0 / bpm) *
			delay_audio->tact);
  delay_audio->notation_delay = (double) delay_audio->frames / (double) devout->buffer_size;
  delay_audio->sequencer_delay = delay_audio->notation_delay * delay_audio->duration;

  delay_audio->bpm = bpm;
}

void
ags_delay_audio_change_tact(AgsTactable *tactable, gdouble tact,
			    AgsDelayAudio *delay_audio)
{
  AgsDevout *devout;

  devout = AGS_DEVOUT(AGS_RECALL_AUDIO(delay_audio)->audio->devout);


  delay_audio->frames = (((double) devout->frequency) *
			(60.0 / delay_audio->bpm) *
			tact);
  delay_audio->notation_delay = (double) delay_audio->frames / (double) devout->buffer_size;
  delay_audio->sequencer_delay = delay_audio->notation_delay * delay_audio->duration;

  delay_audio->tact = tact;
}

void
ags_delay_audio_change_duration(AgsTactable *tactable, gdouble duration,
				AgsDelayAudio *delay_audio)
{
  AgsDevout *devout;

  devout = AGS_DEVOUT(AGS_RECALL_AUDIO(delay_audio)->audio->devout);

  delay_audio->sequencer_delay = (guint) delay_audio->notation_delay * duration;

  delay_audio->duration = duration;
}

AgsDelayAudio*
ags_delay_audio_new(AgsTactable *tactable)
{
  AgsDelayAudio *delay_audio;

  delay_audio = (AgsDelayAudio *) g_object_new(AGS_TYPE_DELAY_AUDIO,
					       "tactable\0", tactable,
					       NULL);

  return(delay_audio);
}
