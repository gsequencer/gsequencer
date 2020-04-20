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

#include <ags/audio/fx/ags_fx_buffer_audio.h>

#include <ags/i18n.h>

void ags_fx_buffer_audio_class_init(AgsFxBufferAudioClass *fx_buffer_audio);
void ags_fx_buffer_audio_init(AgsFxBufferAudio *fx_buffer_audio);
void ags_fx_buffer_audio_set_property(GObject *gobject,
				      guint prop_id,
				      const GValue *value,
				      GParamSpec *param_spec);
void ags_fx_buffer_audio_get_property(GObject *gobject,
				      guint prop_id,
				      GValue *value,
				      GParamSpec *param_spec);
void ags_fx_buffer_audio_dispose(GObject *gobject);
void ags_fx_buffer_audio_finalize(GObject *gobject);

/**
 * SECTION:ags_fx_buffer_audio
 * @short_description: fx buffer audio
 * @title: AgsFxBufferAudio
 * @section_id:
 * @include: ags/audio/fx/ags_fx_buffer_audio.h
 *
 * The #AgsFxBufferAudio class provides ports to the effect processor.
 */

static gpointer ags_fx_buffer_audio_parent_class = NULL;

static const gchar *ags_fx_buffer_audio_plugin_name = "ags-fx-buffer";

static const gchar *ags_fx_buffer_audio_specifier[] = {
  "./available-matrix[0]",
  "./conversion-matrix[0]",
  "./completed-matrix[0]",
  NULL,
};

static const gchar *ags_fx_buffer_audio_control_port[] = {
  "1/3",
  "2/3",
  "3/3",
  NULL,
};

enum{
  PROP_0,
  PROP_AVAILABLE_MATRIX,
  PROP_CONVERSION_MATRIX,
  PROP_COMPLETED_MATRIX,
};

GType
ags_fx_buffer_audio_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fx_buffer_audio = 0;

    static const GTypeInfo ags_fx_buffer_audio_info = {
      sizeof (AgsFxBufferAudioClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_buffer_audio_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio */
      sizeof (AgsFxBufferAudio),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_buffer_audio_init,
    };

    ags_type_fx_buffer_audio = g_type_register_static(AGS_TYPE_RECALL_AUDIO,
						      "AgsFxBufferAudio",
						      &ags_fx_buffer_audio_info,
						      0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fx_buffer_audio);
  }

  return g_define_type_id__volatile;
}

void
ags_fx_buffer_audio_class_init(AgsFxBufferAudioClass *fx_buffer_audio)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_fx_buffer_audio_parent_class = g_type_class_peek_parent(fx_buffer_audio);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_buffer_audio;

  gobject->set_property = ags_fx_buffer_audio_set_property;
  gobject->get_property = ags_fx_buffer_audio_get_property;

  gobject->dispose = ags_fx_buffer_audio_dispose;
  gobject->finalize = ags_fx_buffer_audio_finalize;

  /* properties */
  /**
   * AgsFxBufferAudio:available-matrix:
   *
   * The beats per minute.
   * 
   * Since: 3.3.0
   */
  param_spec = g_param_spec_object("available-matrix",
				   i18n_pspec("available matrix of recall"),
				   i18n_pspec("The recall's available matrix"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AVAILABLE_MATRIX,
				  param_spec);

  /**
   * AgsFxBufferAudio:conversion-matrix:
   *
   * The beats per minute.
   * 
   * Since: 3.3.0
   */
  param_spec = g_param_spec_object("conversion-matrix",
				   i18n_pspec("conversion matrix of recall"),
				   i18n_pspec("The recall's conversion matrix"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CONVERSION_MATRIX,
				  param_spec);

  /**
   * AgsFxBufferAudio:completed-matrix:
   *
   * The beats per minute.
   * 
   * Since: 3.3.0
   */
  param_spec = g_param_spec_object("completed-matrix",
				   i18n_pspec("completed matrix of recall"),
				   i18n_pspec("The recall's completed matrix"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_COMPLETED_MATRIX,
				  param_spec);
}

void
ags_fx_buffer_audio_init(AgsFxBufferAudio *fx_buffer_audio)
{
  AGS_RECALL(fx_buffer_audio)->name = "ags-fx-buffer";
  AGS_RECALL(fx_buffer_audio)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_buffer_audio)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_buffer_audio)->xml_type = "ags-fx-buffer-audio";

  fx_buffer_audio->flags = 0;
  
  /* available matrix */
  fx_buffer_audio->available_matrix = g_object_new(AGS_TYPE_PORT,
					 "plugin-name", ags_fx_buffer_audio_plugin_name,
					 "specifier", ags_fx_buffer_audio_specifier[0],
					 "control-port", ags_fx_buffer_audio_control_port[0],
					 "port-value-is-pointer", TRUE,
					 "port-value-type", G_TYPE_POINTER,
					 "port-value-size", sizeof(gboolean),
					 "port-value-length", AGS_FX_BUFFER_AUDIO_MATRIX_SIZE,
					 NULL);
  g_object_ref(fx_buffer_audio->available_matrix);
  
  fx_buffer_audio->available_matrix->port_value.ags_port_pointer = (gboolean *) g_malloc(AGS_FX_BUFFER_AUDIO_MATRIX_SIZE * sizeof(gboolean));

  ags_recall_add_port((AgsRecall *) fx_buffer_audio,
		      fx_buffer_audio->available_matrix);
  
  /* conversion matrix */
  fx_buffer_audio->conversion_matrix = g_object_new(AGS_TYPE_PORT,
					 "plugin-name", ags_fx_buffer_audio_plugin_name,
					 "specifier", ags_fx_buffer_audio_specifier[1],
					 "control-port", ags_fx_buffer_audio_control_port[1],
					 "port-value-is-pointer", TRUE,
					 "port-value-type", G_TYPE_POINTER,
					 "port-value-size", sizeof(gboolean),
					 "port-value-length", AGS_FX_BUFFER_AUDIO_MATRIX_SIZE,
					 NULL);
  g_object_ref(fx_buffer_audio->conversion_matrix);
  
  fx_buffer_audio->conversion_matrix->port_value.ags_port_pointer = (gboolean *) g_malloc(AGS_FX_BUFFER_AUDIO_MATRIX_SIZE * sizeof(gboolean));

  ags_recall_add_port((AgsRecall *) fx_buffer_audio,
		      fx_buffer_audio->conversion_matrix);
  
  /* completed matrix */
  fx_buffer_audio->completed_matrix = g_object_new(AGS_TYPE_PORT,
					 "plugin-name", ags_fx_buffer_audio_plugin_name,
					 "specifier", ags_fx_buffer_audio_specifier[2],
					 "control-port", ags_fx_buffer_audio_control_port[2],
					 "port-value-is-pointer", TRUE,
					 "port-value-type", G_TYPE_POINTER,
					 "port-value-size", sizeof(gboolean),
					 "port-value-length", AGS_FX_BUFFER_AUDIO_MATRIX_SIZE,
					 NULL);
  g_object_ref(fx_buffer_audio->completed_matrix);
  
  fx_buffer_audio->completed_matrix->port_value.ags_port_pointer = (gboolean *) g_malloc(AGS_FX_BUFFER_AUDIO_MATRIX_SIZE * sizeof(gboolean));

  ags_recall_add_port((AgsRecall *) fx_buffer_audio,
		      fx_buffer_audio->completed_matrix);
}

void
ags_fx_buffer_audio_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec)
{
  AgsFxBufferAudio *fx_buffer_audio;

  GRecMutex *recall_mutex;
  
  fx_buffer_audio = AGS_FX_BUFFER_AUDIO(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_buffer_audio);

  switch(prop_id){
  case PROP_AVAILABLE_MATRIX:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_buffer_audio->available_matrix){
      g_rec_mutex_unlock(recall_mutex);

      return;
    }

    if(fx_buffer_audio->available_matrix != NULL){
      g_object_unref(G_OBJECT(fx_buffer_audio->available_matrix));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_buffer_audio->available_matrix = port;

    g_rec_mutex_unlock(recall_mutex);
  }
  break;
  case PROP_CONVERSION_MATRIX:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_buffer_audio->conversion_matrix){
      g_rec_mutex_unlock(recall_mutex);

      return;
    }

    if(fx_buffer_audio->conversion_matrix != NULL){
      g_object_unref(G_OBJECT(fx_buffer_audio->conversion_matrix));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_buffer_audio->conversion_matrix = port;

    g_rec_mutex_unlock(recall_mutex);
  }
  break;
  case PROP_COMPLETED_MATRIX:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_buffer_audio->completed_matrix){
      g_rec_mutex_unlock(recall_mutex);

      return;
    }

    if(fx_buffer_audio->completed_matrix != NULL){
      g_object_unref(G_OBJECT(fx_buffer_audio->completed_matrix));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_buffer_audio->completed_matrix = port;

    g_rec_mutex_unlock(recall_mutex);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_fx_buffer_audio_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec)
{
  AgsFxBufferAudio *fx_buffer_audio;
  
  GRecMutex *recall_mutex;

  fx_buffer_audio = AGS_FX_BUFFER_AUDIO(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_buffer_audio);

  switch(prop_id){
  case PROP_AVAILABLE_MATRIX:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_buffer_audio->available_matrix);

    g_rec_mutex_unlock(recall_mutex);
  }
  break;
  case PROP_CONVERSION_MATRIX:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_buffer_audio->conversion_matrix);

    g_rec_mutex_unlock(recall_mutex);
  }
  break;
  case PROP_COMPLETED_MATRIX:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_buffer_audio->completed_matrix);

    g_rec_mutex_unlock(recall_mutex);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_fx_buffer_audio_dispose(GObject *gobject)
{
  AgsFxBufferAudio *fx_buffer_audio;
  
  fx_buffer_audio = AGS_FX_BUFFER_AUDIO(gobject);

  if(fx_buffer_audio->available_matrix != NULL){
    g_object_unref(fx_buffer_audio->available_matrix);

    fx_buffer_audio->available_matrix = NULL;
  }

  if(fx_buffer_audio->conversion_matrix != NULL){
    g_object_unref(fx_buffer_audio->conversion_matrix);

    fx_buffer_audio->conversion_matrix = NULL;
  }

  if(fx_buffer_audio->completed_matrix != NULL){
    g_object_unref(fx_buffer_audio->completed_matrix);

    fx_buffer_audio->completed_matrix = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_fx_buffer_audio_parent_class)->dispose(gobject);
}

void
ags_fx_buffer_audio_finalize(GObject *gobject)
{
  AgsFxBufferAudio *fx_buffer_audio;
  
  fx_buffer_audio = AGS_FX_BUFFER_AUDIO(gobject);
  
  if(fx_buffer_audio->available_matrix != NULL){
    g_object_unref(fx_buffer_audio->available_matrix);
  }
  
  if(fx_buffer_audio->conversion_matrix != NULL){
    g_object_unref(fx_buffer_audio->conversion_matrix);
  }
  
  if(fx_buffer_audio->completed_matrix != NULL){
    g_object_unref(fx_buffer_audio->completed_matrix);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_fx_buffer_audio_parent_class)->finalize(gobject);
}

/**
 * ags_fx_buffer_audio_new:
 * @audio: the #AgsAudio
 *
 * Create a new instance of #AgsFxBufferAudio
 *
 * Returns: the new #AgsFxBufferAudio
 *
 * Since: 3.3.0
 */
AgsFxBufferAudio*
ags_fx_buffer_audio_new(AgsAudio *audio)
{
  AgsFxBufferAudio *fx_buffer_audio;

  fx_buffer_audio = (AgsFxBufferAudio *) g_object_new(AGS_TYPE_FX_BUFFER_AUDIO,
						      "audio", audio,
						      NULL);

  return(fx_buffer_audio);
}
