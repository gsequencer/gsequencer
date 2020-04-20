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

#include <ags/audio/fx/ags_fx_pattern_audio.h>

#include <ags/audio/ags_pattern.h>

#include <ags/i18n.h>

void ags_fx_pattern_audio_class_init(AgsFxPatternAudioClass *fx_pattern_audio);
void ags_fx_pattern_audio_init(AgsFxPatternAudio *fx_pattern_audio);
void ags_fx_pattern_audio_set_property(GObject *gobject,
				       guint prop_id,
				       const GValue *value,
				       GParamSpec *param_spec);
void ags_fx_pattern_audio_get_property(GObject *gobject,
				       guint prop_id,
				       GValue *value,
				       GParamSpec *param_spec);
void ags_fx_pattern_audio_dispose(GObject *gobject);
void ags_fx_pattern_audio_finalize(GObject *gobject);

/**
 * SECTION:ags_fx_pattern_audio
 * @short_description: fx pattern audio
 * @title: AgsFxPatternAudio
 * @section_id:
 * @include: ags/audio/fx/ags_fx_pattern_audio.h
 *
 * The #AgsFxPatternAudio class provides ports to the effect processor.
 */

static gpointer ags_fx_pattern_audio_parent_class = NULL;

static const gchar *ags_fx_pattern_audio_plugin_name = "ags-fx-pattern";

static const gchar *ags_fx_pattern_audio_specifier[] = {
  "./bpm[0]",
  "./tact[0]",
  "./delay[0]",
  "./duration[0]"
  "./loop[0]",
  "./loop-start[0]",
  "./loop-end[0]"
  "./bank-index-0[0]",
  "./bank-index-1[0]",
  NULL,
};

static const gchar *ags_fx_pattern_audio_control_port[] = {
  "1/9",
  "2/9",
  "3/9",
  "4/9",
  "5/9",
  "6/9",
  "7/9",
  "8/9",
  "9/9",
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
  PROP_BANK_INDEX_0,
  PROP_BANK_INDEX_1,
};

GType
ags_fx_pattern_audio_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fx_pattern_audio = 0;

    static const GTypeInfo ags_fx_pattern_audio_info = {
      sizeof (AgsFxPatternAudioClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_pattern_audio_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio */
      sizeof (AgsFxPatternAudio),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_pattern_audio_init,
    };

    ags_type_fx_pattern_audio = g_type_register_static(AGS_TYPE_RECALL_AUDIO,
						       "AgsFxPatternAudio",
						       &ags_fx_pattern_audio_info,
						       0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fx_pattern_audio);
  }

  return g_define_type_id__volatile;
}

void
ags_fx_pattern_audio_class_init(AgsFxPatternAudioClass *fx_pattern_audio)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_fx_pattern_audio_parent_class = g_type_class_peek_parent(fx_pattern_audio);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_pattern_audio;

  gobject->set_property = ags_fx_pattern_audio_set_property;
  gobject->get_property = ags_fx_pattern_audio_get_property;

  gobject->dispose = ags_fx_pattern_audio_dispose;
  gobject->finalize = ags_fx_pattern_audio_finalize;

  /* properties */
  /**
   * AgsFxPatternAudio:bpm:
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
   * AgsFxPatternAudio:tact:
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
   * AgsFxPatternAudio:delay:
   *
   * The pattern's delay.
   * 
   * Since: 3.3.0
   */
  param_spec = g_param_spec_object("delay",
				   i18n_pspec("delay of recall"),
				   i18n_pspec("The delay of the pattern"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DELAY,
				  param_spec);

  /**
   * AgsFxPatternAudio:duration:
   *
   * The pattern's duration.
   * 
   * Since: 3.3.0
   */
  param_spec = g_param_spec_object("duration",
				   i18n_pspec("duration of recall"),
				   i18n_pspec("The duration of the pattern"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DURATION,
				  param_spec);

  /**
   * AgsFxPatternAudio:loop:
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
   * AgsFxPatternAudio:loop-start:
   *
   * The pattern's loop-start.
   * 
   * Since: 3.3.0
   */
  param_spec = g_param_spec_object("loop-start",
				   i18n_pspec("start beat of pattern loop"),
				   i18n_pspec("The start beat of the pattern loop"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_LOOP_START,
				  param_spec);

  /**
   * AgsFxPatternAudio:loop-end:
   *
   * The pattern's loop-end.
   * 
   * Since: 3.3.0
   */
  param_spec = g_param_spec_object("loop-end",
				   i18n_pspec("end beat of pattern loop"),
				   i18n_pspec("The end beat of the pattern loop"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_LOOP_END,
				  param_spec);

  /**
   * AgsFxPatternAudio:bank-index-0:
   *
   * Selected bank index 0.
   * 
   * Since: 3.3.0
   */
  param_spec = g_param_spec_object("bank-index-0",
				   i18n_pspec("bank index 0"),
				   i18n_pspec("Selected bank index 0"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BANK_INDEX_0,
				  param_spec);

  /**
   * AgsFxPatternAudio:bank-index-1:
   *
   * Selected bank index 1.
   * 
   * Since: 3.3.0
   */
  param_spec = g_param_spec_object("bank-index-1",
				   i18n_pspec("bank index 1"),
				   i18n_pspec("Selected bank index 1"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BANK_INDEX_1,
				  param_spec);
}

void
ags_fx_pattern_audio_init(AgsFxPatternAudio *fx_pattern_audio)
{
  gdouble bpm;
  gdouble delay;

  AGS_RECALL(fx_pattern_audio)->name = "ags-fx-pattern";
  AGS_RECALL(fx_pattern_audio)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_pattern_audio)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_pattern_audio)->xml_type = "ags-fx-pattern-audio";

  fx_pattern_audio->flags = 0;
  
  bpm = AGS_SOUNDCARD_DEFAULT_BPM;
  delay = AGS_SOUNDCARD_DEFAULT_DELAY;

  fx_pattern_audio->note = NULL;
  
  /* bpm */
  fx_pattern_audio->bpm = g_object_new(AGS_TYPE_PORT,
				       "plugin-name", ags_fx_pattern_audio_plugin_name,
				       "specifier", ags_fx_pattern_audio_specifier[0],
				       "control-port", ags_fx_pattern_audio_control_port[0],
				       "port-value-is-pointer", FALSE,
				       "port-value-type", G_TYPE_DOUBLE,
				       "port-value-size", sizeof(gdouble),
				       "port-value-length", 1,
				       NULL);
  g_object_ref(fx_pattern_audio->bpm);
  
  fx_pattern_audio->bpm->port_value.ags_port_double = bpm;

  ags_recall_add_port((AgsRecall *) fx_pattern_audio,
		      fx_pattern_audio->bpm);
  
  /* tact */
  fx_pattern_audio->tact = g_object_new(AGS_TYPE_PORT,
					"plugin-name", ags_fx_pattern_audio_plugin_name,
					"specifier", ags_fx_pattern_audio_specifier[1],
					"control-port", ags_fx_pattern_audio_control_port[1],
					"port-value-is-pointer", FALSE,
					"port-value-type", G_TYPE_DOUBLE,
					"port-value-size", sizeof(gdouble),
					"port-value-length", 1,
					NULL);
  g_object_ref(fx_pattern_audio->tact);
  
  fx_pattern_audio->tact->port_value.ags_port_double = AGS_SOUNDCARD_DEFAULT_TACT;

  ags_recall_add_port((AgsRecall *) fx_pattern_audio,
		      fx_pattern_audio->tact);

  /* delay */
  fx_pattern_audio->delay = g_object_new(AGS_TYPE_PORT,
					 "plugin-name", ags_fx_pattern_audio_plugin_name,
					 "specifier", ags_fx_pattern_audio_specifier[2],
					 "control-port", ags_fx_pattern_audio_control_port[2],
					 "port-value-is-pointer", FALSE,
					 "port-value-type", G_TYPE_DOUBLE,
					 "port-value-size", sizeof(gdouble),
					 "port-value-length", 1,
					 NULL);
  g_object_ref(fx_pattern_audio->delay);
  
  fx_pattern_audio->delay->port_value.ags_port_double = delay;

  ags_recall_add_port((AgsRecall *) fx_pattern_audio,
		      fx_pattern_audio->delay);
  
  /* duration */
  fx_pattern_audio->duration = g_object_new(AGS_TYPE_PORT,
					    "plugin-name", ags_fx_pattern_audio_plugin_name,
					    "specifier", ags_fx_pattern_audio_specifier[3],
					    "control-port", ags_fx_pattern_audio_control_port[3],
					    "port-value-is-pointer", FALSE,
					    "port-value-type", G_TYPE_UINT64,
					    "port-value-size", sizeof(guint64),
					    "port-value-length", 1,
					    NULL);
  g_object_ref(fx_pattern_audio->duration);
  
  fx_pattern_audio->duration->port_value.ags_port_double = (gdouble) AGS_PATTERN_DEFAULT_OFFSET;

  ags_recall_add_port((AgsRecall *) fx_pattern_audio,
		      fx_pattern_audio->duration);
  
  /* loop */
  fx_pattern_audio->loop = g_object_new(AGS_TYPE_PORT,
					"plugin-name", ags_fx_pattern_audio_plugin_name,
					"specifier", ags_fx_pattern_audio_specifier[4],
					"control-port", ags_fx_pattern_audio_control_port[4],
					"port-value-is-pointer", FALSE,
					"port-value-type", G_TYPE_BOOLEAN,
					"port-value-size", sizeof(gboolean),
					"port-value-length", 1,
					NULL);
  g_object_ref(fx_pattern_audio->loop);
  
  fx_pattern_audio->loop->port_value.ags_port_boolean = FALSE;

  ags_recall_add_port((AgsRecall *) fx_pattern_audio,
		      fx_pattern_audio->loop);

  /* loop-start */
  fx_pattern_audio->loop_start = g_object_new(AGS_TYPE_PORT,
					      "plugin-name", ags_fx_pattern_audio_plugin_name,
					      "specifier", ags_fx_pattern_audio_specifier[5],
					      "control-port", ags_fx_pattern_audio_control_port[5],
					      "port-value-is-pointer", FALSE,
					      "port-value-type", G_TYPE_UINT64,
					      "port-value-size", sizeof(guint64),
					      "port-value-length", 1,
					      NULL);
  g_object_ref(fx_pattern_audio->loop_start);
  
  fx_pattern_audio->loop_start->port_value.ags_port_uint = AGS_FX_PATTERN_AUDIO_DEFAULT_LOOP_START;

  ags_recall_add_port((AgsRecall *) fx_pattern_audio,
		      fx_pattern_audio->loop_start);
  
  /* loop-end */
  fx_pattern_audio->loop_end = g_object_new(AGS_TYPE_PORT,
					    "plugin-name", ags_fx_pattern_audio_plugin_name,
					    "specifier", ags_fx_pattern_audio_specifier[6],
					    "control-port", ags_fx_pattern_audio_control_port[6],
					    "port-value-is-pointer", FALSE,
					    "port-value-type", G_TYPE_UINT64,
					    "port-value-size", sizeof(guint64),
					    "port-value-length", 1,
					    NULL);
  g_object_ref(fx_pattern_audio->loop_end);
  
  fx_pattern_audio->loop_end->port_value.ags_port_uint = AGS_FX_PATTERN_AUDIO_DEFAULT_LOOP_END;

  ags_recall_add_port((AgsRecall *) fx_pattern_audio,
		      fx_pattern_audio->loop_end);

  /* bank-index 0 */
  fx_pattern_audio->bank_index_0 = g_object_new(AGS_TYPE_PORT,
						"plugin-name", ags_fx_pattern_audio_plugin_name,
						"specifier", ags_fx_pattern_audio_specifier[7],
						"control-port", ags_fx_pattern_audio_control_port[7],
						"port-value-is-pointer", FALSE,
						"port-value-type", G_TYPE_FLOAT,
						"port-value-size", sizeof(gfloat),
						"port-value-length", 1,
						NULL);
  g_object_ref(fx_pattern_audio->bank_index_0);
  
  fx_pattern_audio->bank_index_0->port_value.ags_port_float = 0.0;

  ags_recall_add_port((AgsRecall *) fx_pattern_audio,
		      fx_pattern_audio->bank_index_0);

  /* bank-index 1 */
  fx_pattern_audio->bank_index_1 = g_object_new(AGS_TYPE_PORT,
						"plugin-name", ags_fx_pattern_audio_plugin_name,
						"specifier", ags_fx_pattern_audio_specifier[8],
						"control-port", ags_fx_pattern_audio_control_port[8],
						"port-value-is-pointer", FALSE,
						"port-value-type", G_TYPE_FLOAT,
						"port-value-size", sizeof(gfloat),
						"port-value-length", 1,
						NULL);
  g_object_ref(fx_pattern_audio->bank_index_1);
  
  fx_pattern_audio->bank_index_1->port_value.ags_port_float = 0.0;

  ags_recall_add_port((AgsRecall *) fx_pattern_audio,
		      fx_pattern_audio->bank_index_1);
}

void
ags_fx_pattern_audio_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec)
{
  AgsFxPatternAudio *fx_pattern_audio;

  GRecMutex *recall_mutex;
  
  fx_pattern_audio = AGS_FX_PATTERN_AUDIO(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_pattern_audio);

  switch(prop_id){
  case PROP_BPM:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_pattern_audio->bpm){
      g_rec_mutex_unlock(recall_mutex);

      return;
    }

    if(fx_pattern_audio->bpm != NULL){
      g_object_unref(G_OBJECT(fx_pattern_audio->bpm));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_pattern_audio->bpm = port;

    g_rec_mutex_unlock(recall_mutex);
  }
  break;
  case PROP_TACT:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_pattern_audio->tact){
      g_rec_mutex_unlock(recall_mutex);

      return;
    }

    if(fx_pattern_audio->tact != NULL){
      g_object_unref(G_OBJECT(fx_pattern_audio->tact));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_pattern_audio->tact = port;

    g_rec_mutex_unlock(recall_mutex);
  }
  break;
  case PROP_DELAY:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_pattern_audio->delay){
      g_rec_mutex_unlock(recall_mutex);

      return;
    }

    if(fx_pattern_audio->delay != NULL){
      g_object_unref(G_OBJECT(fx_pattern_audio->delay));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_pattern_audio->delay = port;

    g_rec_mutex_unlock(recall_mutex);
  }
  break;
  case PROP_DURATION:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_pattern_audio->duration){
      g_rec_mutex_unlock(recall_mutex);

      return;
    }

    if(fx_pattern_audio->duration != NULL){
      g_object_unref(G_OBJECT(fx_pattern_audio->duration));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_pattern_audio->duration = port;

    g_rec_mutex_unlock(recall_mutex);
  }
  break;
  case PROP_LOOP:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_pattern_audio->loop){
      g_rec_mutex_unlock(recall_mutex);

      return;
    }

    if(fx_pattern_audio->loop != NULL){
      g_object_unref(G_OBJECT(fx_pattern_audio->loop));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_pattern_audio->loop = port;

    g_rec_mutex_unlock(recall_mutex);
  }
  break;
  case PROP_LOOP_START:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_pattern_audio->loop_start){
      g_rec_mutex_unlock(recall_mutex);

      return;
    }

    if(fx_pattern_audio->loop_start != NULL){
      g_object_unref(G_OBJECT(fx_pattern_audio->loop_start));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_pattern_audio->loop_start = port;

    g_rec_mutex_unlock(recall_mutex);
  }
  break;
  case PROP_LOOP_END:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_pattern_audio->loop_end){
      g_rec_mutex_unlock(recall_mutex);

      return;
    }

    if(fx_pattern_audio->loop_end != NULL){
      g_object_unref(G_OBJECT(fx_pattern_audio->loop_end));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_pattern_audio->loop_end = port;

    g_rec_mutex_unlock(recall_mutex);
  }
  break;
  case PROP_BANK_INDEX_0:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_pattern_audio->bank_index_0){
      g_rec_mutex_unlock(recall_mutex);

      return;
    }

    if(fx_pattern_audio->bank_index_0 != NULL){
      g_object_unref(G_OBJECT(fx_pattern_audio->bank_index_0));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_pattern_audio->bank_index_0 = port;

    g_rec_mutex_unlock(recall_mutex);
  }
  break;
  case PROP_BANK_INDEX_1:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_pattern_audio->bank_index_1){
      g_rec_mutex_unlock(recall_mutex);

      return;
    }

    if(fx_pattern_audio->bank_index_1 != NULL){
      g_object_unref(G_OBJECT(fx_pattern_audio->bank_index_1));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_pattern_audio->bank_index_1 = port;

    g_rec_mutex_unlock(recall_mutex);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_fx_pattern_audio_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec)
{
  AgsFxPatternAudio *fx_pattern_audio;
  
  GRecMutex *recall_mutex;

  fx_pattern_audio = AGS_FX_PATTERN_AUDIO(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_pattern_audio);

  switch(prop_id){
  case PROP_BPM:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_pattern_audio->bpm);

    g_rec_mutex_unlock(recall_mutex);
  }
  break;
  case PROP_TACT:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_pattern_audio->tact);

    g_rec_mutex_unlock(recall_mutex);
  }
  break;
  case PROP_DELAY:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_pattern_audio->delay);

    g_rec_mutex_unlock(recall_mutex);
  }
  break;
  case PROP_DURATION:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_pattern_audio->duration);

    g_rec_mutex_unlock(recall_mutex);
  }
  break;
  case PROP_LOOP:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_pattern_audio->loop);

    g_rec_mutex_unlock(recall_mutex);
  }
  break;
  case PROP_LOOP_START:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_pattern_audio->loop_start);

    g_rec_mutex_unlock(recall_mutex);
  }
  break;
  case PROP_LOOP_END:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_pattern_audio->loop_end);

    g_rec_mutex_unlock(recall_mutex);
  }
  break;
  case PROP_BANK_INDEX_0:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_pattern_audio->bank_index_0);

    g_rec_mutex_unlock(recall_mutex);
  }
  break;
  case PROP_BANK_INDEX_1:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_pattern_audio->bank_index_1);

    g_rec_mutex_unlock(recall_mutex);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_fx_pattern_audio_dispose(GObject *gobject)
{
  AgsFxPatternAudio *fx_pattern_audio;
  
  fx_pattern_audio = AGS_FX_PATTERN_AUDIO(gobject);

  if(fx_pattern_audio->bpm != NULL){
    g_object_unref(fx_pattern_audio->bpm);

    fx_pattern_audio->bpm = NULL;
  }

  if(fx_pattern_audio->tact != NULL){
    g_object_unref(fx_pattern_audio->tact);

    fx_pattern_audio->tact = NULL;
  }

  if(fx_pattern_audio->delay != NULL){
    g_object_unref(fx_pattern_audio->delay);

    fx_pattern_audio->delay = NULL;
  }

  if(fx_pattern_audio->duration != NULL){
    g_object_unref(fx_pattern_audio->duration);

    fx_pattern_audio->duration = NULL;
  }

  if(fx_pattern_audio->loop != NULL){
    g_object_unref(fx_pattern_audio->loop);

    fx_pattern_audio->loop = NULL;
  }

  if(fx_pattern_audio->loop_start != NULL){
    g_object_unref(fx_pattern_audio->loop_start);

    fx_pattern_audio->loop_start = NULL;
  }

  if(fx_pattern_audio->loop_end != NULL){
    g_object_unref(fx_pattern_audio->loop_end);

    fx_pattern_audio->loop_end = NULL;
  }

  if(fx_pattern_audio->bank_index_0 != NULL){
    g_object_unref(fx_pattern_audio->bank_index_0);

    fx_pattern_audio->bank_index_0 = NULL;
  }

  if(fx_pattern_audio->bank_index_1 != NULL){
    g_object_unref(fx_pattern_audio->bank_index_1);

    fx_pattern_audio->bank_index_1 = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_pattern_audio_parent_class)->dispose(gobject);
}

void
ags_fx_pattern_audio_finalize(GObject *gobject)
{
  AgsFxPatternAudio *fx_pattern_audio;
  
  fx_pattern_audio = AGS_FX_PATTERN_AUDIO(gobject);
  
  if(fx_pattern_audio->bpm != NULL){
    g_object_unref(fx_pattern_audio->bpm);
  }

  if(fx_pattern_audio->tact != NULL){
    g_object_unref(fx_pattern_audio->tact);
  }

  if(fx_pattern_audio->delay != NULL){
    g_object_unref(fx_pattern_audio->delay);
  }

  if(fx_pattern_audio->duration != NULL){
    g_object_unref(fx_pattern_audio->duration);
  }

  if(fx_pattern_audio->loop != NULL){
    g_object_unref(fx_pattern_audio->loop);
  }

  if(fx_pattern_audio->loop_start != NULL){
    g_object_unref(fx_pattern_audio->loop_start);
  }

  if(fx_pattern_audio->loop_end != NULL){
    g_object_unref(fx_pattern_audio->loop_end);
  }

  if(fx_pattern_audio->bank_index_0 != NULL){
    g_object_unref(fx_pattern_audio->bank_index_0);
  }

  if(fx_pattern_audio->bank_index_1 != NULL){
    g_object_unref(fx_pattern_audio->bank_index_1);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_fx_pattern_audio_parent_class)->finalize(gobject);
}

/**
 * ags_fx_pattern_audio_new:
 * @audio: the #AgsAudio
 *
 * Create a new instance of #AgsFxPatternAudio
 *
 * Returns: the new #AgsFxPatternAudio
 *
 * Since: 3.3.0
 */
AgsFxPatternAudio*
ags_fx_pattern_audio_new(AgsAudio *audio)
{
  AgsFxPatternAudio *fx_pattern_audio;

  fx_pattern_audio = (AgsFxPatternAudio *) g_object_new(AGS_TYPE_FX_PATTERN_AUDIO,
							"audio", audio,
							NULL);

  return(fx_pattern_audio);
}
