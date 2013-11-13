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

#include <ags/audio/ags_devout.h>

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
  PROP_TACT,
  PROP_BPM,
  PROP_SEQUENCER_DURATION,
  PROP_NOTATION_DURATION,
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
  param_spec = g_param_spec_object("bpm\0",
				   "bpm of recall\0",
				   "The recall's bpm\0",
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BPM,
				  param_spec);

  param_spec = g_param_spec_object("tact\0",
				   "tact of recall\0",
				   "The recall's tact\0",
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_TACT,
				  param_spec);

  param_spec = g_param_spec_object("sequencer-delay\0",
				   "sequencer-delay of recall\0",
				   "The delay of the sequencer\0",
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SEQUENCER_DELAY,
				  param_spec);

  param_spec = g_param_spec_object("notation-delay\0",
				   "notation-delay of recall\0",
				   "The delay of the notation\0",
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_NOTATION_DELAY,
				  param_spec);

  param_spec = g_param_spec_object("sequencer-duration\0",
				   "sequencer-duration of recall\0",
				   "The duration of the sequencer\0",
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SEQUENCER_DURATION,
				  param_spec);

  param_spec = g_param_spec_object("notation-duration\0",
				   "notation-duration of recall\0",
				   "The duration of the notation\0",
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_NOTATION_DURATION,
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
  GList *port;

  port = NULL;

  /* bpm */
  delay_audio->bpm = g_object_new(AGS_TYPE_PORT,
				  "plugin-name\0", g_strdup("ags-delay\0"),
				  "specifier\0", "./bpm[0]\0",
				  "control-port\0", "0/6\0",
				  "port-value-is-pointer\0", FALSE,
				  "value-type\0", G_TYPE_DOUBLE,
				  "value-size\0", sizeof(gdouble),
				  "value-length", 1,
				  NULL);

  delay_audio->bpm->port_value.ags_port_double = AGS_DEVOUT_DEFAULT_BPM;

  port = g_list_prepend(port, delay_audio->bpm);


  /* tact */
  delay_audio->tact = g_object_new(AGS_TYPE_PORT,
				   "plugin-name\0", g_strdup("ags-delay\0"),
				   "specifier\0", "./tact[0]\0",
				   "control-port\0", "0/6\0",
				   "port-value-is-pointer\0", FALSE,
				   "value-type\0", G_TYPE_DOUBLE,
				   "value-size\0", sizeof(gdouble),
				   "value-length", 1,
				   NULL);

  delay_audio->tact->port_value.ags_port_double = AGS_DEVOUT_DEFAULT_TACT;

  port = g_list_prepend(port, delay_audio->tact);

  /* sequencer delay */
  delay_audio->sequencer_delay = g_object_new(AGS_TYPE_PORT,
					      "plugin-name\0", g_strdup("ags-delay\0"),
					      "specifier\0", "./sequencer_delay[0]\0",
					      "control-port\0", "0/6\0",
					      "port-value-is-pointer\0", FALSE,
					      "value-type\0", G_TYPE_DOUBLE,
					      "value-size\0", sizeof(gdouble),
					      "value-length", 1,
					      NULL);

  delay_audio->sequencer_delay->port_value.ags_port_double = AGS_DEVOUT_DEFAULT_DELAY;

  port = g_list_prepend(port, delay_audio->sequencer_delay);

  /* notation delay */
  delay_audio->notation_delay = g_object_new(AGS_TYPE_PORT,
					     "plugin-name\0", g_strdup("ags-delay\0"),
					     "specifier\0", "./notation_delay[0]\0",
					     "control-port\0", "0/6\0",
					     "port-value-is-pointer\0", FALSE,
					     "value-type\0", G_TYPE_DOUBLE,
					     "value-size\0", sizeof(gdouble),
					     "value-length", 1,
					     NULL);

  delay_audio->notation_delay->port_value.ags_port_double = AGS_DEVOUT_DEFAULT_DELAY;

  port = g_list_prepend(port, delay_audio->notation_delay);

  /* sequencer duration */
  delay_audio->sequencer_duration = g_object_new(AGS_TYPE_PORT,
						 "plugin-name\0", g_strdup("ags-duration\0"),
						 "specifier\0", "./sequencer_duration[0]\0",
						 "control-port\0", "0/6\0",
						 "port-value-is-pointer\0", FALSE,
						 "value-type\0", G_TYPE_DOUBLE,
						 "value-size\0", sizeof(gdouble),
						 "value-length", 1,
						 NULL);

  delay_audio->sequencer_duration->port_value.ags_port_double = AGS_NOTATION_DEFAULT_DURATION;

  port = g_list_prepend(port, delay_audio->sequencer_duration);

  /* notation duration */
  delay_audio->notation_duration = g_object_new(AGS_TYPE_PORT,
						"plugin-name\0", g_strdup("ags-duration\0"),
						"specifier\0", "./notation_duration[0]\0",
						"control-port\0", "0/6\0",
						"port-value-is-pointer\0", FALSE,
						"value-type\0", G_TYPE_DOUBLE,
						"value-size\0", sizeof(gdouble),
						"value-length", 1,
						NULL);

  delay_audio->notation_duration->port_value.ags_port_double = AGS_NOTATION_DEFAULT_DURATION;

  port = g_list_prepend(port, delay_audio->notation_duration);
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
  case PROP_BPM:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      if(port == delay_audio->bpm){
	return;
      }

      if(delay_audio->bpm != NULL){
	g_object_unref(G_OBJECT(delay_audio->bpm));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      delay_audio->bpm = port;
    }
    break;
  case PROP_TACT:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      if(port == delay_audio->tact){
	return;
      }

      if(delay_audio->tact != NULL){
	g_object_unref(G_OBJECT(delay_audio->tact));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      delay_audio->tact = port;
    }
    break;
  case PROP_NOTATION_DELAY:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      if(port == delay_audio->notation_delay){
	return;
      }

      if(delay_audio->notation_delay != NULL){
	g_object_unref(G_OBJECT(delay_audio->notation_delay));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      delay_audio->notation_delay = port;
    }
    break;
  case PROP_SEQUENCER_DELAY:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      if(port == delay_audio->sequencer_delay){
	return;
      }

      if(delay_audio->sequencer_delay != NULL){
	g_object_unref(G_OBJECT(delay_audio->sequencer_delay));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      delay_audio->sequencer_delay = port;
    }
    break;
  case PROP_NOTATION_DURATION:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      if(port == delay_audio->notation_duration){
	return;
      }

      if(delay_audio->notation_duration != NULL){
	g_object_unref(G_OBJECT(delay_audio->notation_duration));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      delay_audio->notation_duration = port;
    }
    break;
  case PROP_SEQUENCER_DURATION:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      if(port == delay_audio->sequencer_duration){
	return;
      }

      if(delay_audio->sequencer_duration != NULL){
	g_object_unref(G_OBJECT(delay_audio->sequencer_duration));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      delay_audio->sequencer_duration = port;
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
  case PROP_BPM:
    g_value_set_object(value, delay_audio->bpm);
    break;
  case PROP_TACT:
    g_value_set_object(value, delay_audio->tact);
    break;
  case PROP_NOTATION_DELAY:
    g_value_set_object(value, delay_audio->notation_delay);
    break;
  case PROP_SEQUENCER_DELAY:
    g_value_set_object(value, delay_audio->sequencer_delay);
    break;
  case PROP_NOTATION_DURATION:
    g_value_set_object(value, delay_audio->notation_duration);
    break;
  case PROP_SEQUENCER_DURATION:
    g_value_set_object(value, delay_audio->sequencer_duration);
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
ags_delay_audio_change_bpm(AgsTactable *tactable, gdouble new_bpm)
{
  AgsDevout *devout;
  AgsDelayAudio *delay_audio;
  gdouble old_bpm;
  gdouble sequencer_delay, notation_delay;
  gdouble sequencer_duration, notation_duration;
  GValue value = {0,};

  delay_audio = AGS_DELAY_AUDIO(tactable);

  devout = AGS_DEVOUT(AGS_RECALL(delay_audio)->devout);

  /* retrieve old bpm */
  g_value_init(&value, G_TYPE_DOUBLE);

  ags_port_safe_read(delay_audio->bpm, &value);
  old_bpm = g_value_get_double(&value);

  /* retrieve sequencer_delay */
  g_value_reset(&value);

  ags_port_safe_read(delay_audio->sequencer_delay, &value);
  sequencer_delay = g_value_get_double(&value);

  /* retrieve notation_delay */
  g_value_reset(&value);

  ags_port_safe_read(delay_audio->notation_delay, &value);
  notation_delay = g_value_get_double(&value);

  /* retrieve sequencer_duration */
  g_value_reset(&value);

  ags_port_safe_read(delay_audio->sequencer_duration, &value);
  sequencer_duration = g_value_get_double(&value);

  /* retrieve notation_duration */
  g_value_reset(&value);

  ags_port_safe_read(delay_audio->notation_duration, &value);
  notation_duration = g_value_get_double(&value);

  /* -- start adjust -- */
  /* notation-delay */
  g_value_reset(&value);

  g_value_set_double(&value, notation_delay * (old_bpm / new_bpm));
  ags_port_safe_write(delay_audio->notation_delay, &value);

  /* sequencer-delay */
  g_value_reset(&value);

  g_value_set_double(&value, sequencer_delay * (old_bpm / new_bpm));
  ags_port_safe_write(delay_audio->sequencer_delay, &value);

  /**/
  g_value_reset(&value);

  g_value_set_double(&value, new_bpm);
  ags_port_safe_write(delay_audio->bpm, &value);

  /* notation-duration */
  g_value_reset(&value);

  g_value_set_double(&value, notation_duration * (old_bpm / new_bpm));
  ags_port_safe_write(delay_audio->notation_duration, &value);

  /* sequencer-duration */
  g_value_reset(&value);

  g_value_set_double(&value, sequencer_duration * (old_bpm / new_bpm));
  ags_port_safe_write(delay_audio->sequencer_duration, &value);

  /* -- finish adjust -- */

  /* emit changed */
  ags_delay_audio_sequencer_duration_changed(delay_audio);
}

void
ags_delay_audio_change_tact(AgsTactable *tactable, gdouble new_tact)
{
  AgsDevout *devout;
  AgsDelayAudio *delay_audio;
  gdouble old_tact;
  gdouble sequencer_delay, notation_delay;
  gdouble sequencer_duration, notation_duration;
  GValue value = {0,};

  delay_audio = AGS_DELAY_AUDIO(tactable);

  devout = AGS_DEVOUT(AGS_RECALL(delay_audio)->devout);

  /* retrieve old tact */
  g_value_init(&value, G_TYPE_DOUBLE);

  ags_port_safe_read(delay_audio->tact, &value);
  old_tact = g_value_get_double(&value);

  /* retrieve sequencer_delay */
  g_value_reset(&value);

  ags_port_safe_read(delay_audio->sequencer_delay, &value);
  sequencer_delay = g_value_get_double(&value);

  /* retrieve notation_delay */
  g_value_reset(&value);

  ags_port_safe_read(delay_audio->notation_delay, &value);
  notation_delay = g_value_get_double(&value);

  /* retrieve sequencer_duration */
  g_value_reset(&value);

  ags_port_safe_read(delay_audio->sequencer_duration, &value);
  sequencer_duration = g_value_get_double(&value);

  /* retrieve notation_duration */
  g_value_reset(&value);

  ags_port_safe_read(delay_audio->notation_duration, &value);
  notation_duration = g_value_get_double(&value);

  /* -- start adjust -- */
  /* notation-delay */
  g_value_reset(&value);

  g_value_set_double(&value, notation_delay * (old_tact / new_tact));
  ags_port_safe_write(delay_audio->notation_delay, &value);

  /* sequencer-delay */
  g_value_reset(&value);

  g_value_set_double(&value, sequencer_delay * (old_tact / new_tact));
  ags_port_safe_write(delay_audio->sequencer_delay, &value);

  /**/
  g_value_reset(&value);

  g_value_set_double(&value, new_tact);
  ags_port_safe_write(delay_audio->tact, &value);

  /* notation-duration */
  g_value_reset(&value);

  g_value_set_double(&value, notation_duration * (old_tact / new_tact));
  ags_port_safe_write(delay_audio->notation_duration, &value);

  /* sequencer-duration */
  g_value_reset(&value);

  g_value_set_double(&value, sequencer_duration * (old_tact / new_tact));
  ags_port_safe_write(delay_audio->sequencer_duration, &value);

  /* -- finish adjust -- */

  /* emit changed */
  ags_delay_audio_sequencer_duration_changed(delay_audio);
}

void
ags_delay_audio_change_sequencer_duration(AgsTactable *tactable, gdouble duration)
{
  AgsDelayAudio *delay_audio;
  GValue value = {0,};
  
  delay_audio = AGS_DELAY_AUDIO(tactable);

  g_value_init(&value, G_TYPE_DOUBLE);

  g_value_set_double(&value, duration);
  ags_delay_audio_sequencer_duration_changed(delay_audio);
}

void
ags_delay_audio_change_notation_duration(AgsTactable *tactable, gdouble duration)
{
  AgsDelayAudio *delay_audio;
  GValue value = {0,};
  
  delay_audio = AGS_DELAY_AUDIO(tactable);

  g_value_init(&value, G_TYPE_DOUBLE);

  g_value_set_double(&value, duration);
  ags_delay_audio_notation_duration_changed(delay_audio);
}

void
ags_delay_audio_sequencer_duration_changed(AgsDelayAudio *delay_audio)
{
  g_return_if_fail(AGS_IS_DELAY_AUDIO(delay_audio));

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
