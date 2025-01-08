/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#include <ags/audio/ags_recall_audio_signal.h>

#include <ags/audio/ags_audio_signal.h>

#include <ags/i18n.h>

void ags_recall_audio_signal_class_init(AgsRecallAudioSignalClass *recall_audio_signal);
void ags_recall_audio_signal_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_recall_audio_signal_init(AgsRecallAudioSignal *recall_audio_signal);
void ags_recall_audio_signal_set_property(GObject *gobject,
					  guint prop_id,
					  const GValue *value,
					  GParamSpec *param_spec);
void ags_recall_audio_signal_get_property(GObject *gobject,
					  guint prop_id,
					  GValue *value,
					  GParamSpec *param_spec);
void ags_recall_audio_signal_dispose(GObject *gobject);
void ags_recall_audio_signal_finalize(GObject *gobject);

/**
 * SECTION:ags_recall_audio_signal
 * @Short_description: The recall base class of dynamic audio signal context
 * @Title: AgsRecallAudioSignal
 * @section_id:
 * @include: ags/audio/ags_recall_audio_signal.h
 *
 * #AgsRecallAudioSignal acts as dynamic audio signal recall. It does audio processing for audio signal context.
 */

enum{
  PROP_0,
  PROP_DESTINATION,
  PROP_SOURCE,
};

static gpointer ags_recall_audio_signal_parent_class = NULL;
static AgsConnectableInterface *ags_recall_audio_signal_parent_connectable_interface;

GType
ags_recall_audio_signal_get_type()
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_recall_audio_signal = 0;

    static const GTypeInfo ags_recall_audio_signal_info = {
      sizeof (AgsRecallAudioSignalClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_recall_audio_signal_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRecallAudioSignal),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_recall_audio_signal_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_recall_audio_signal_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_recall_audio_signal = g_type_register_static(AGS_TYPE_RECALL,
							  "AgsRecallAudioSignal",
							  &ags_recall_audio_signal_info,
							  0);

    g_type_add_interface_static(ags_type_recall_audio_signal,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__static, ags_type_recall_audio_signal);
  }

  return(g_define_type_id__static);
}

void
ags_recall_audio_signal_class_init(AgsRecallAudioSignalClass *recall_audio_signal)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  GParamSpec *param_spec;

  ags_recall_audio_signal_parent_class = g_type_class_peek_parent(recall_audio_signal);

  /* GObjectClass */
  gobject = (GObjectClass *) recall_audio_signal;

  gobject->set_property = ags_recall_audio_signal_set_property;
  gobject->get_property = ags_recall_audio_signal_get_property;

  gobject->dispose = ags_recall_audio_signal_dispose;
  gobject->finalize = ags_recall_audio_signal_finalize;

  /* properties */
  /**
   * AgsRecallAudioSignal:destination:
   *
   * The destination audio signal
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("destination",
				   i18n_pspec("destination of output"),
				   i18n_pspec("The destination where this recall will write the audio signal to"),
				   AGS_TYPE_AUDIO_SIGNAL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DESTINATION,
				  param_spec);

  /**
   * AgsRecallAudioSignal:source:
   *
   * The source audio signal
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("source",
				   i18n_pspec("source of input"),
				   i18n_pspec("The source where this recall will take the audio signal from"),
				   AGS_TYPE_AUDIO_SIGNAL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SOURCE,
				  param_spec);
}

void
ags_recall_audio_signal_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_recall_audio_signal_parent_connectable_interface = g_type_interface_peek_parent(connectable);
}

void
ags_recall_audio_signal_init(AgsRecallAudioSignal *recall_audio_signal)
{
  recall_audio_signal->source = NULL;
  recall_audio_signal->destination = NULL;
}

void
ags_recall_audio_signal_set_property(GObject *gobject,
				     guint prop_id,
				     const GValue *value,
				     GParamSpec *param_spec)
{
  AgsRecallAudioSignal *recall_audio_signal;

  GRecMutex *recall_mutex;

  recall_audio_signal = AGS_RECALL_AUDIO_SIGNAL(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall_audio_signal);

  switch(prop_id){
  case PROP_DESTINATION:
    {
      AgsAudioSignal *destination;

      destination = (AgsAudioSignal *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(recall_audio_signal->destination == destination){
	g_rec_mutex_unlock(recall_mutex);

	return;
      }

      if(recall_audio_signal->destination != NULL){
	g_object_unref(recall_audio_signal->destination);
      }
      
      if(destination != NULL){
	g_object_ref(G_OBJECT(destination));
      }
      
      recall_audio_signal->destination = destination;

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_SOURCE:
    {
      AgsAudioSignal *source;

      source = (AgsAudioSignal *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(recall_audio_signal->source == source){
	g_rec_mutex_unlock(recall_mutex);
	
	return;
      }

      if(recall_audio_signal->source != NULL){
	g_object_unref(recall_audio_signal->source);
      }
      
      if(source != NULL){	
	g_object_ref(G_OBJECT(source));
      }
      
      recall_audio_signal->source = source;

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_recall_audio_signal_get_property(GObject *gobject,
				     guint prop_id,
				     GValue *value,
				     GParamSpec *param_spec)
{
  AgsRecallAudioSignal *recall_audio_signal;

  GRecMutex *recall_mutex;

  recall_audio_signal = AGS_RECALL_AUDIO_SIGNAL(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall_audio_signal);

  switch(prop_id){
  case PROP_DESTINATION:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, recall_audio_signal->destination);

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_SOURCE:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, recall_audio_signal->source);

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_recall_audio_signal_dispose(GObject *gobject)
{
  AgsRecallAudioSignal *recall_audio_signal;

  recall_audio_signal = AGS_RECALL_AUDIO_SIGNAL(gobject);

  /* destination */
  if(recall_audio_signal->destination != NULL){
    g_object_unref(recall_audio_signal->destination);

    recall_audio_signal->destination = NULL;
  }

  /* source */
  if(recall_audio_signal->source != NULL){
    g_object_unref(recall_audio_signal->source);

    recall_audio_signal->source = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_recall_audio_signal_parent_class)->dispose(gobject);
}

void
ags_recall_audio_signal_finalize(GObject *gobject)
{
  AgsRecallAudioSignal *recall_audio_signal;

  recall_audio_signal = AGS_RECALL_AUDIO_SIGNAL(gobject);

#ifdef AGS_DEBUG
  g_message("- recall audio signal %s", G_OBJECT_TYPE_NAME(recall_audio_signal));
#endif
  
  /* destination */
  if(recall_audio_signal->destination != NULL){
    g_object_unref(recall_audio_signal->destination);
  }

  /* source */
  if(recall_audio_signal->source != NULL){
    g_object_unref(recall_audio_signal->source);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_recall_audio_signal_parent_class)->finalize(gobject);
}

/**
 * ags_recall_audio_signal_new:
 *
 * Creates an #AgsRecallAudioSignal.
 *
 * Returns: a new #AgsRecallAudioSignal.
 *
 * Since: 3.0.0
 */
AgsRecallAudioSignal*
ags_recall_audio_signal_new()
{
  AgsRecallAudioSignal *recall_audio_signal;

  recall_audio_signal = (AgsRecallAudioSignal *) g_object_new(AGS_TYPE_RECALL_AUDIO_SIGNAL,
							      NULL);

  return(recall_audio_signal);
}
