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

#include <ags/audio/fx/ags_fx_notation_audio.h>

#include <ags/i18n.h>

void ags_fx_notation_audio_class_init(AgsFxNotationAudioClass *fx_notation_audio);
void ags_fx_notation_audio_init(AgsFxNotationAudio *fx_notation_audio);
void ags_fx_notation_audio_set_property(GObject *gobject,
					guint prop_id,
					const GValue *value,
					GParamSpec *param_spec);
void ags_fx_notation_audio_get_property(GObject *gobject,
					guint prop_id,
					GValue *value,
					GParamSpec *param_spec);
void ags_fx_notation_audio_dispose(GObject *gobject);
void ags_fx_notation_audio_finalize(GObject *gobject);

/**
 * SECTION:ags_fx_notation_audio
 * @short_description: fx notation audio
 * @title: AgsFxNotationAudio
 * @section_id:
 * @include: ags/audio/fx/ags_fx_notation_audio.h
 *
 * The #AgsFxNotationAudio class provides ports to the effect processor.
 */

static gpointer ags_fx_notation_audio_parent_class = NULL;

static const gchar *ags_fx_notation_audio_plugin_name = "ags-fx-notation";

static const gchar *ags_fx_notation_audio_specifier[] = {
  "./bpm[0]",
  "./tact[0]",
  "./delay[0]",
  "./duration[0]"
  "./loop[0]",
  "./loop-start[0]",
  "./loop-end[0]",
  NULL,
};

static const gchar *ags_fx_notation_audio_control_port[] = {
  "1/7",
  "2/7",
  "3/7",
  "4/7",
  "5/7",
  "6/7",
  "7/7",
  NULL,
};

enum{
  PROP_0,
  PROP_DELAY_COUNTER,
  PROP_OFFSET_COUNTER,
  PROP_TACT,
  PROP_BPM,
  PROP_DELAY,
  PROP_DURATION,
  PROP_LOOP,
  PROP_LOOP_START,
  PROP_LOOP_END,
};

GType
ags_fx_notation_audio_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fx_notation_audio = 0;

    static const GTypeInfo ags_fx_notation_audio_info = {
      sizeof (AgsFxNotationAudioClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_notation_audio_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio */
      sizeof (AgsFxNotationAudio),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_notation_audio_init,
    };

    ags_type_fx_notation_audio = g_type_register_static(AGS_TYPE_RECALL_AUDIO,
							"AgsFxNotationAudio",
							&ags_fx_notation_audio_info,
							0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fx_notation_audio);
  }

  return g_define_type_id__volatile;
}

void
ags_fx_notation_audio_class_init(AgsFxNotationAudioClass *fx_notation_audio)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_fx_notation_audio_parent_class = g_type_class_peek_parent(fx_notation_audio);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_notation_audio;

  gobject->set_property = ags_fx_notation_audio_set_property;
  gobject->get_property = ags_fx_notation_audio_get_property;

  gobject->dispose = ags_fx_notation_audio_dispose;
  gobject->finalize = ags_fx_notation_audio_finalize;

  /* properties */
  /**
   * AgsFxNotationAudio:bpm:
   *
   * The beats per minute.
   * 
   * Since: 3.3.0
   */
  param_spec = g_param_spec_object("bpm",
				   i18n_pspec("bpm of recall"),
				   i18n_pspec("The recall's bpm"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BPM,
				  param_spec);

  /**
   * AgsFxNotationAudio:tact:
   *
   * The tact segmentation.
   * 
   * Since: 3.3.0
   */
  param_spec = g_param_spec_object("tact",
				   i18n_pspec("tact of recall"),
				   i18n_pspec("The recall's tact"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_TACT,
				  param_spec);

  /**
   * AgsFxNotationAudio:delay:
   *
   * The notation's delay.
   * 
   * Since: 3.3.0
   */
  param_spec = g_param_spec_object("delay",
				   i18n_pspec("delay of recall"),
				   i18n_pspec("The delay of the notation"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DELAY,
				  param_spec);

  /**
   * AgsFxNotationAudio:duration:
   *
   * The notation's duration.
   * 
   * Since: 3.3.0
   */
  param_spec = g_param_spec_object("duration",
				   i18n_pspec("duration of recall"),
				   i18n_pspec("The duration of the notation"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DURATION,
				  param_spec);

  /**
   * AgsFxNotationAudio:loop:
   *
   * Do loop.
   * 
   * Since: 3.3.0
   */
  param_spec = g_param_spec_object("loop",
				   i18n_pspec("loop playing"),
				   i18n_pspec("Play in a infinite loop"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_LOOP,
				  param_spec);

  /**
   * AgsFxNotationAudio:loop-start:
   *
   * The notation's loop-start.
   * 
   * Since: 3.3.0
   */
  param_spec = g_param_spec_object("loop-start",
				   i18n_pspec("start beat of notation loop"),
				   i18n_pspec("The start beat of the notation loop"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_LOOP_START,
				  param_spec);

  /**
   * AgsFxNotationAudio:loop-end:
   *
   * The notation's loop-end.
   * 
   * Since: 3.3.0
   */
  param_spec = g_param_spec_object("loop-end",
				   i18n_pspec("end beat of notation loop"),
				   i18n_pspec("The end beat of the notation loop"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_LOOP_END,
				  param_spec);
}

void
ags_fx_notation_audio_init(AgsFxNotationAudio *fx_notation_audio)
{
  gdouble bpm;
  gdouble delay;

  AGS_RECALL(fx_notation_audio)->name = "ags-fx-notation";
  AGS_RECALL(fx_notation_audio)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_notation_audio)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_notation_audio)->xml_type = "ags-fx-notation-audio";

  fx_notation_audio->flags = 0;
  
  fx_notation_audio->feed_note = NULL;

  bpm = AGS_SOUNDCARD_DEFAULT_BPM;
  delay = AGS_SOUNDCARD_DEFAULT_DELAY;

  /* bpm */
  fx_notation_audio->bpm = g_object_new(AGS_TYPE_PORT,
					"plugin-name", ags_fx_notation_audio_plugin_name,
					"specifier", ags_fx_notation_audio_specifier[0],
					"control-port", ags_fx_notation_audio_control_port[0],
					"port-value-is-pointer", FALSE,
					"port-value-type", G_TYPE_DOUBLE,
					"port-value-size", sizeof(gdouble),
					"port-value-length", 1,
					NULL);
  g_object_ref(fx_notation_audio->bpm);
  
  fx_notation_audio->bpm->port_value.ags_port_double = bpm;

  ags_recall_add_port((AgsRecall *) fx_notation_audio,
		      fx_notation_audio->bpm);
  
  /* tact */
  fx_notation_audio->tact = g_object_new(AGS_TYPE_PORT,
					 "plugin-name", ags_fx_notation_audio_plugin_name,
					 "specifier", ags_fx_notation_audio_specifier[1],
					 "control-port", ags_fx_notation_audio_control_port[1],
					 "port-value-is-pointer", FALSE,
					 "port-value-type", G_TYPE_DOUBLE,
					 "port-value-size", sizeof(gdouble),
					 "port-value-length", 1,
					 NULL);
  g_object_ref(fx_notation_audio->tact);
  
  fx_notation_audio->tact->port_value.ags_port_double = AGS_SOUNDCARD_DEFAULT_TACT;

  ags_recall_add_port((AgsRecall *) fx_notation_audio,
		      fx_notation_audio->tact);

  /* delay */
  fx_notation_audio->delay = g_object_new(AGS_TYPE_PORT,
					  "plugin-name", ags_fx_notation_audio_plugin_name,
					  "specifier", ags_fx_notation_audio_specifier[2],
					  "control-port", ags_fx_notation_audio_control_port[2],
					  "port-value-is-pointer", FALSE,
					  "port-value-type", G_TYPE_DOUBLE,
					  "port-value-size", sizeof(gdouble),
					  "port-value-length", 1,
					  NULL);
  g_object_ref(fx_notation_audio->delay);
  
  fx_notation_audio->delay->port_value.ags_port_double = delay;

  ags_recall_add_port((AgsRecall *) fx_notation_audio,
		      fx_notation_audio->delay);
  
  /* duration */
  fx_notation_audio->duration = g_object_new(AGS_TYPE_PORT,
					     "plugin-name", ags_fx_notation_audio_plugin_name,
					     "specifier", ags_fx_notation_audio_specifier[3],
					     "control-port", ags_fx_notation_audio_control_port[3],
					     "port-value-is-pointer", FALSE,
					     "port-value-type", G_TYPE_UINT64,
					     "port-value-size", sizeof(guint64),
					     "port-value-length", 1,
					     NULL);
  g_object_ref(fx_notation_audio->duration);
  
  fx_notation_audio->duration->port_value.ags_port_double = ceil(AGS_NOTATION_DEFAULT_DURATION * delay);

  ags_recall_add_port((AgsRecall *) fx_notation_audio,
		      fx_notation_audio->duration);

  /* loop */
  fx_notation_audio->loop = g_object_new(AGS_TYPE_PORT,
					 "plugin-name", ags_fx_notation_audio_plugin_name,
					 "specifier", ags_fx_notation_audio_specifier[4],
					 "control-port", ags_fx_notation_audio_control_port[4],
					 "port-value-is-pointer", FALSE,
					 "port-value-type", G_TYPE_BOOLEAN,
					 "port-value-size", sizeof(gboolean),
					 "port-value-length", 1,
					 NULL);
  g_object_ref(fx_notation_audio->loop);
  
  fx_notation_audio->loop->port_value.ags_port_boolean = FALSE;

  ags_recall_add_port((AgsRecall *) fx_notation_audio,
		      fx_notation_audio->loop);

  /* loop-start */
  fx_notation_audio->loop_start = g_object_new(AGS_TYPE_PORT,
					       "plugin-name", ags_fx_notation_audio_plugin_name,
					       "specifier", ags_fx_notation_audio_specifier[5],
					       "control-port", ags_fx_notation_audio_control_port[5],
					       "port-value-is-pointer", FALSE,
					       "port-value-type", G_TYPE_UINT64,
					       "port-value-size", sizeof(guint64),
					       "port-value-length", 1,
					       NULL);
  g_object_ref(fx_notation_audio->loop_start);
  
  fx_notation_audio->loop_start->port_value.ags_port_uint = AGS_FX_NOTATION_AUDIO_DEFAULT_LOOP_START;

  ags_recall_add_port((AgsRecall *) fx_notation_audio,
		      fx_notation_audio->loop_start);
  
  /* loop-end */
  fx_notation_audio->loop_end = g_object_new(AGS_TYPE_PORT,
					     "plugin-name", ags_fx_notation_audio_plugin_name,
					     "specifier", ags_fx_notation_audio_specifier[6],
					     "control-port", ags_fx_notation_audio_control_port[6],
					     "port-value-is-pointer", FALSE,
					     "port-value-type", G_TYPE_UINT64,
					     "port-value-size", sizeof(guint64),
					     "port-value-length", 1,
					     NULL);
  g_object_ref(fx_notation_audio->loop_end);
  
  fx_notation_audio->loop_end->port_value.ags_port_uint = AGS_FX_NOTATION_AUDIO_DEFAULT_LOOP_END;

  ags_recall_add_port((AgsRecall *) fx_notation_audio,
		      fx_notation_audio->loop_end);
}

void
ags_fx_notation_audio_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec)
{
  AgsFxNotationAudio *fx_notation_audio;

  GRecMutex *recall_mutex;
  
  fx_notation_audio = AGS_FX_NOTATION_AUDIO(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_notation_audio);

  switch(prop_id){
  case PROP_BPM:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_notation_audio->bpm){
	g_rec_mutex_unlock(recall_mutex);

	return;
      }

      if(fx_notation_audio->bpm != NULL){
	g_object_unref(G_OBJECT(fx_notation_audio->bpm));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_notation_audio->bpm = port;

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_TACT:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_notation_audio->tact){
	g_rec_mutex_unlock(recall_mutex);

	return;
      }

      if(fx_notation_audio->tact != NULL){
	g_object_unref(G_OBJECT(fx_notation_audio->tact));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_notation_audio->tact = port;

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_DELAY:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_notation_audio->delay){
	g_rec_mutex_unlock(recall_mutex);

	return;
      }

      if(fx_notation_audio->delay != NULL){
	g_object_unref(G_OBJECT(fx_notation_audio->delay));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_notation_audio->delay = port;

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_DURATION:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_notation_audio->duration){
	g_rec_mutex_unlock(recall_mutex);

	return;
      }

      if(fx_notation_audio->duration != NULL){
	g_object_unref(G_OBJECT(fx_notation_audio->duration));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_notation_audio->duration = port;

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_LOOP:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_notation_audio->loop){
	g_rec_mutex_unlock(recall_mutex);

	return;
      }

      if(fx_notation_audio->loop != NULL){
	g_object_unref(G_OBJECT(fx_notation_audio->loop));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_notation_audio->loop = port;

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_LOOP_START:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_notation_audio->loop_start){
	g_rec_mutex_unlock(recall_mutex);

	return;
      }

      if(fx_notation_audio->loop_start != NULL){
	g_object_unref(G_OBJECT(fx_notation_audio->loop_start));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_notation_audio->loop_start = port;

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_LOOP_END:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_notation_audio->loop_end){
	g_rec_mutex_unlock(recall_mutex);

	return;
      }

      if(fx_notation_audio->loop_end != NULL){
	g_object_unref(G_OBJECT(fx_notation_audio->loop_end));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_notation_audio->loop_end = port;

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_fx_notation_audio_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec)
{
  AgsFxNotationAudio *fx_notation_audio;
  
  GRecMutex *recall_mutex;

  fx_notation_audio = AGS_FX_NOTATION_AUDIO(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_notation_audio);

  switch(prop_id){
  case PROP_BPM:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_notation_audio->bpm);

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_TACT:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_notation_audio->tact);

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_DELAY:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_notation_audio->delay);

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_DURATION:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_notation_audio->duration);

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_LOOP:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_notation_audio->loop);

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_LOOP_START:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_notation_audio->loop_start);

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_LOOP_END:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_notation_audio->loop_end);

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_fx_notation_audio_dispose(GObject *gobject)
{
  AgsFxNotationAudio *fx_notation_audio;
  
  fx_notation_audio = AGS_FX_NOTATION_AUDIO(gobject);

  if(fx_notation_audio->feed_note != NULL){
    g_list_free_full(fx_notation_audio->feed_note,
		     (GDestroyNotify) g_object_unref);

    fx_notation_audio->feed_note = NULL;
  }

  if(fx_notation_audio->bpm != NULL){
    g_object_unref(fx_notation_audio->bpm);

    fx_notation_audio->bpm = NULL;
  }

  if(fx_notation_audio->tact != NULL){
    g_object_unref(fx_notation_audio->tact);

    fx_notation_audio->tact = NULL;
  }

  if(fx_notation_audio->delay != NULL){
    g_object_unref(fx_notation_audio->delay);

    fx_notation_audio->delay = NULL;
  }

  if(fx_notation_audio->duration != NULL){
    g_object_unref(fx_notation_audio->duration);

    fx_notation_audio->duration = NULL;
  }

  if(fx_notation_audio->loop != NULL){
    g_object_unref(fx_notation_audio->loop);

    fx_notation_audio->loop = NULL;
  }

  if(fx_notation_audio->loop_start != NULL){
    g_object_unref(fx_notation_audio->loop_start);

    fx_notation_audio->loop_start = NULL;
  }

  if(fx_notation_audio->loop_end != NULL){
    g_object_unref(fx_notation_audio->loop_end);

    fx_notation_audio->loop_end = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_notation_audio_parent_class)->dispose(gobject);
}

void
ags_fx_notation_audio_finalize(GObject *gobject)
{
  AgsFxNotationAudio *fx_notation_audio;
  
  fx_notation_audio = AGS_FX_NOTATION_AUDIO(gobject);

  if(fx_notation_audio->feed_note != NULL){
    g_list_free_full(fx_notation_audio->feed_note,
		     (GDestroyNotify) g_object_unref);
  }
  
  if(fx_notation_audio->bpm != NULL){
    g_object_unref(fx_notation_audio->bpm);
  }

  if(fx_notation_audio->tact != NULL){
    g_object_unref(fx_notation_audio->tact);
  }

  if(fx_notation_audio->delay != NULL){
    g_object_unref(fx_notation_audio->delay);
  }

  if(fx_notation_audio->duration != NULL){
    g_object_unref(fx_notation_audio->duration);
  }

  if(fx_notation_audio->loop != NULL){
    g_object_unref(fx_notation_audio->loop);
  }

  if(fx_notation_audio->loop_start != NULL){
    g_object_unref(fx_notation_audio->loop_start);
  }

  if(fx_notation_audio->loop_end != NULL){
    g_object_unref(fx_notation_audio->loop_end);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_fx_notation_audio_parent_class)->finalize(gobject);
}

/**
 * ags_fx_notation_audio_test_flags:
 * @fx_notation_audio: the #AgsFxNotationAudio
 * @flags: the flags
 * 
 * Test @flags of @fx_notation_audio.
 * 
 * Returns: %TRUE on success, otherwise %FALSE
 * 
 * Since: 3.3.0
 */
gboolean
ags_fx_notation_audio_test_flags(AgsFxNotationAudio *fx_notation_audio, guint flags)
{
  gboolean success;
  
  GRecMutex *recall_mutex;
  
  if(!AGS_IS_FX_NOTATION_AUDIO(fx_notation_audio)){
    return(FALSE);
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_notation_audio);

  /* test flags */
  g_rec_mutex_lock(recall_mutex);

  success = ((flags & (fx_notation_audio->flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(recall_mutex);

  return(success);
}

/**
 * ags_fx_notation_audio_set_flags:
 * @fx_notation_audio: the #AgsFxNotationAudio
 * @flags: the flags
 * 
 * Set @flags of @fx_notation_audio.
 * 
 * Since: 3.3.0
 */
void
ags_fx_notation_audio_set_flags(AgsFxNotationAudio *fx_notation_audio, guint flags)
{  
  GRecMutex *recall_mutex;

  if(!AGS_IS_FX_NOTATION_AUDIO(fx_notation_audio)){
    return;
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_notation_audio);

  /* set flags */
  g_rec_mutex_lock(recall_mutex);

  fx_notation_audio->flags |= flags;

  g_rec_mutex_unlock(recall_mutex);
}

/**
 * ags_fx_notation_audio_unset_flags:
 * @fx_notation_audio: the #AgsFxNotationAudio
 * @flags: the flags
 * 
 * Unset @flags of @fx_notation_audio.
 * 
 * Since: 3.3.0
 */
void
ags_fx_notation_audio_unset_flags(AgsFxNotationAudio *fx_notation_audio, guint flags)
{  
  GRecMutex *recall_mutex;

  if(!AGS_IS_FX_NOTATION_AUDIO(fx_notation_audio)){
    return;
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_notation_audio);

  /* unset flags */
  g_rec_mutex_lock(recall_mutex);

  fx_notation_audio->flags &= (~flags);

  g_rec_mutex_unlock(recall_mutex);
}

/**
 * ags_fx_notation_audio_get_feed_note:
 * @fx_notation_audio: the #AgsFxNotationAudio
 * 
 * Get feed note of @fx_notation_audio.
 * 
 * Returns: (element-type AgsAudio.Note) (transfer full): the #GList-struct containing feed note
 * 
 * Since: 3.3.0
 */
GList*
ags_fx_notation_audio_get_feed_note(AgsFxNotationAudio *fx_notation_audio)
{
  GList *feed_note;
  
  GRecMutex *recall_mutex;

  if(!AGS_IS_FX_NOTATION_AUDIO(fx_notation_audio)){
    return(NULL);
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_notation_audio);

  /* unset flags */
  g_rec_mutex_lock(recall_mutex);

  feed_note = g_list_copy_deep(fx_notation_audio->feed_note,
			       (GCopyFunc) g_object_ref,
			       NULL);

  g_rec_mutex_unlock(recall_mutex);

  return(feed_note);
}

/**
 * ags_fx_notation_audio_add_feed_note:
 * @fx_notation_audio: the #AgsFxNotationAudio
 * @feed_note: the #AgsNote
 * 
 * Add @feed_note to @fx_notation_audio.
 * 
 * Since: 3.3.0
 */
void
ags_fx_notation_audio_add_feed_note(AgsFxNotationAudio *fx_notation_audio,
				    AgsNote *feed_note)
{
  GRecMutex *recall_mutex;

  if(!AGS_IS_FX_NOTATION_AUDIO(fx_notation_audio)){
    return;
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_notation_audio);

  /* add */
  g_rec_mutex_lock(recall_mutex);

  if(g_list_find(fx_notation_audio->feed_note, feed_note) == NULL){
    g_object_ref(feed_note);

    fx_notation_audio->feed_note = g_list_prepend(fx_notation_audio->feed_note,
						  feed_note);
  }
  
  g_rec_mutex_unlock(recall_mutex);
}

/**
 * ags_fx_notation_audio_remove_feed_note:
 * @fx_notation_audio: the #AgsFxNotationAudio
 * @feed_note: the #AgsNote
 * 
 * Remove @feed_note from @fx_notation_audio.
 * 
 * Since: 3.3.0
 */
void
ags_fx_notation_audio_remove_feed_note(AgsFxNotationAudio *fx_notation_audio,
				       AgsNote *feed_note)
{
  GRecMutex *recall_mutex;

  if(!AGS_IS_FX_NOTATION_AUDIO(fx_notation_audio)){
    return;
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_notation_audio);

  /* remove */
  g_rec_mutex_lock(recall_mutex);

  if(g_list_find(fx_notation_audio->feed_note, feed_note) != NULL){
    fx_notation_audio->feed_note = g_list_remove(fx_notation_audio->feed_note,
						 feed_note);
    g_object_unref(feed_note);
  }
  
  g_rec_mutex_unlock(recall_mutex);
}

/**
 * ags_fx_notation_audio_new:
 * @audio: the #AgsAudio
 *
 * Create a new instance of #AgsFxNotationAudio
 *
 * Returns: the new #AgsFxNotationAudio
 *
 * Since: 3.3.0
 */
AgsFxNotationAudio*
ags_fx_notation_audio_new(AgsAudio *audio)
{
  AgsFxNotationAudio *fx_notation_audio;

  fx_notation_audio = (AgsFxNotationAudio *) g_object_new(AGS_TYPE_FX_NOTATION_AUDIO,
							  "audio", audio,
							  NULL);

  return(fx_notation_audio);
}
