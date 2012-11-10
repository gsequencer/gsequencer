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

#include <ags/object/ags_tactable.h>

void ags_delay_audio_class_init(AgsDelayAudioClass *delay_audio);
void ags_delay_audio_tactable_interface_init(AgsTactableInterface *tactable);
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

void ags_delay_audio_change_bpm(AgsTactable *tactable, gdouble bpm);
void ags_delay_audio_change_tact(AgsTactable *tactable, gdouble tact);
void ags_delay_audio_change_sequencer_duration(AgsTactable *tactable, gdouble duration);
void ags_delay_audio_change_notation_duration(AgsTactable *tactable, gdouble duration);

enum{
  SEQUENCER_DURATION_CHANGED,
  LAST_SIGNAL,
};

enum{
  PROP_0,
  PROP_NOTATION_DELAY,
  PROP_SEQUENCER_DELAY,
};

static gpointer ags_delay_audio_parent_class = NULL;

static guint delay_audio_signals[LAST_SIGNAL];

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

    static const GInterfaceInfo ags_tactable_interface_info = {
      (GInterfaceInitFunc) ags_delay_audio_tactable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_delay_audio = g_type_register_static(AGS_TYPE_RECALL_AUDIO,
						  "AgsDelayAudio\0",
						  &ags_delay_audio_info,
						  0);

    g_type_add_interface_static(ags_type_delay_audio,
				AGS_TYPE_TACTABLE,
				&ags_tactable_interface_info);
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

  /* signals */
  delay_audio_signals[SEQUENCER_DURATION_CHANGED] = 
    g_signal_new("sequencer_duration_changed\0",
		 G_TYPE_FROM_CLASS(delay_audio),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsDelayAudioClass, sequencer_duration_changed),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);
}

void
ags_delay_audio_tactable_interface_init(AgsTactableInterface *tactable)
{
  tactable->change_sequencer_duration = ags_delay_audio_change_sequencer_duration;
  tactable->change_notation_duration = ags_delay_audio_change_notation_duration;
  tactable->change_tact = ags_delay_audio_change_tact;
  tactable->change_bpm = ags_delay_audio_change_bpm;
}

void
ags_delay_audio_init(AgsDelayAudio *delay_audio)
{
  delay_audio->bpm = 120.0;
  delay_audio->tact = 1.0 / 4.0;

  delay_audio->notation_delay = 44100.0 / 940.0 * (60.0 / delay_audio->bpm) / 64.0;
  delay_audio->sequencer_delay = delay_audio->notation_delay * (64.0 * delay_audio->tact);

  delay_audio->sequencer_duration = 16.0;
  delay_audio->notation_duration = 1200.0 * 64.0;
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

void
ags_delay_audio_change_bpm(AgsTactable *tactable, gdouble bpm)
{
  AgsDevout *devout;
  AgsDelayAudio *delay_audio;
  
  delay_audio = AGS_DELAY_AUDIO(tactable);

  devout = AGS_DEVOUT(AGS_RECALL(delay_audio)->devout);

  delay_audio->notation_delay *= (delay_audio->bpm / bpm);
  delay_audio->sequencer_delay *= (delay_audio->bpm / bpm);

  delay_audio->bpm = bpm;

  delay_audio->sequencer_duration *= (delay_audio->bpm / bpm);
  ags_delay_audio_sequencer_duration_changed(delay_audio);
}

void
ags_delay_audio_change_tact(AgsTactable *tactable, gdouble tact)
{
  AgsDevout *devout;
  AgsDelayAudio *delay_audio;
  
  delay_audio = AGS_DELAY_AUDIO(tactable);

  devout = AGS_DEVOUT(AGS_RECALL(delay_audio)->devout);

  delay_audio->notation_delay *= (delay_audio->tact / tact);
  delay_audio->sequencer_delay *= (delay_audio->tact / tact);

  delay_audio->tact = tact;

  delay_audio->sequencer_duration *= (delay_audio->tact / tact);
  ags_delay_audio_sequencer_duration_changed(delay_audio);
}

void
ags_delay_audio_change_sequencer_duration(AgsTactable *tactable, gdouble duration)
{
  AgsDevout *devout;
  AgsDelayAudio *delay_audio;
  
  delay_audio = AGS_DELAY_AUDIO(tactable);

  devout = AGS_DEVOUT(AGS_RECALL_AUDIO(delay_audio)->audio->devout);

  delay_audio->sequencer_duration = duration;
  ags_delay_audio_sequencer_duration_changed(delay_audio);
}

void
ags_delay_audio_change_notation_duration(AgsTactable *tactable, gdouble duration)
{
  AgsDevout *devout;
  AgsDelayAudio *delay_audio;
  
  delay_audio = AGS_DELAY_AUDIO(tactable);

  devout = AGS_DEVOUT(AGS_RECALL_AUDIO(delay_audio)->audio->devout);

  delay_audio->notation_duration = duration;
}

void
ags_delay_audio_sequencer_duration_changed(AgsDelayAudio *delay_audio)
{
  g_return_if_fail(AGS_IS_AUDIO(delay_audio));

  g_object_ref((GObject *) delay_audio);
  g_signal_emit(G_OBJECT(delay_audio),
		delay_audio_signals[SEQUENCER_DURATION_CHANGED], 0);
  g_object_unref((GObject *) delay_audio);
}

AgsDelayAudio*
ags_delay_audio_new()
{
  AgsDelayAudio *delay_audio;

  delay_audio = (AgsDelayAudio *) g_object_new(AGS_TYPE_DELAY_AUDIO,
					       NULL);

  return(delay_audio);
}
