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

#include <ags/audio/ags_recall_audio_run.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_recall_audio.h>
#include <ags/audio/ags_recall_channel_run.h>
#include <ags/audio/ags_recall_container.h>

#include <ags/i18n.h>

void ags_recall_audio_run_class_init(AgsRecallAudioRunClass *recall_audio_run);
void ags_recall_audio_run_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_recall_audio_run_init(AgsRecallAudioRun *recall_audio_run);
void ags_recall_audio_run_set_property(GObject *gobject,
				       guint prop_id,
				       const GValue *value,
				       GParamSpec *param_spec);
void ags_recall_audio_run_get_property(GObject *gobject,
				       guint prop_id,
				       GValue *value,
				       GParamSpec *param_spec);
void ags_recall_audio_run_dispose(GObject *gobject);
void ags_recall_audio_run_finalize(GObject *gobject);

void ags_recall_audio_run_notify_recall_container_callback(GObject *gobject,
							   GParamSpec *pspec,
							   gpointer user_data);

AgsRecall* ags_recall_audio_run_duplicate(AgsRecall *recall,
					  AgsRecallID *recall_id,
					  guint *n_params, gchar **parameter_name, GValue *value);

/**
 * SECTION:ags_recall_audio_run
 * @short_description: The recall base class of dynamic audio context
 * @title: AgsRecallAudioRun
 * @section_id:
 * @include: ags/audio/ags_recall_audio_run.h
 *
 * #AgsRecallAudioRun acts as dynamic audio recall. It does audio processing for audio context.
 */

enum{
  PROP_0,
  PROP_AUDIO,
  PROP_RECALL_AUDIO,
};

static gpointer ags_recall_audio_run_parent_class = NULL;
static AgsConnectableInterface* ags_recall_audio_run_parent_connectable_interface;

GType
ags_recall_audio_run_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_recall_audio_run = 0;

    static const GTypeInfo ags_recall_audio_run_info = {
      sizeof (AgsRecallAudioRunClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_recall_audio_run_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRecallAudioRun),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_recall_audio_run_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_recall_audio_run_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_recall_audio_run = g_type_register_static(AGS_TYPE_RECALL,
						       "AgsRecallAudioRun",
						       &ags_recall_audio_run_info,
						       0);

    g_type_add_interface_static(ags_type_recall_audio_run,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_recall_audio_run);
  }

  return g_define_type_id__volatile;
}

void
ags_recall_audio_run_class_init(AgsRecallAudioRunClass *recall_audio_run)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  GParamSpec *param_spec;

  ags_recall_audio_run_parent_class = g_type_class_peek_parent(recall_audio_run);

  /* GObjectClass */
  gobject = (GObjectClass *) recall_audio_run;

  gobject->set_property = ags_recall_audio_run_set_property;
  gobject->get_property = ags_recall_audio_run_get_property;

  gobject->dispose = ags_recall_audio_run_dispose;
  gobject->finalize = ags_recall_audio_run_finalize;

  /* properties */
  /**
   * AgsRecallAudioRun:audio:
   *
   * The assigned audio.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("audio",
				   i18n_pspec("assigned audio"),
				   i18n_pspec("The audio object it is assigned to"),
				   AGS_TYPE_AUDIO,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO,
				  param_spec);

  /**
   * AgsRecallAudioRun:recall-audio:
   *
   * The recall audio belonging to.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("recall-audio",
				   i18n_pspec("AgsRecallAudio of this recall"),
				   i18n_pspec("The AgsRecallAudio which this recall needs"),
				   AGS_TYPE_RECALL_AUDIO,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RECALL_AUDIO,
				  param_spec);

  /* AgsRecallClass */
  recall = (AgsRecallClass *) recall_audio_run;

  recall->duplicate = ags_recall_audio_run_duplicate;
}

void
ags_recall_audio_run_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_recall_audio_run_parent_connectable_interface = g_type_interface_peek_parent(connectable);
}

void
ags_recall_audio_run_init(AgsRecallAudioRun *recall_audio_run)
{  
  g_signal_connect_after(recall_audio_run, "notify::recall-container",
			 G_CALLBACK(ags_recall_audio_run_notify_recall_container_callback), NULL);

  recall_audio_run->audio = NULL;
  recall_audio_run->recall_audio = NULL;
}

void
ags_recall_audio_run_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec)
{
  AgsRecallAudioRun *recall_audio_run;

  GRecMutex *recall_mutex;

  recall_audio_run = AGS_RECALL_AUDIO_RUN(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall_audio_run);

  switch(prop_id){
  case PROP_AUDIO:
    {
      AgsAudio *audio;

      audio = (AgsRecallAudio *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(recall_audio_run->audio == audio){
	g_rec_mutex_unlock(recall_mutex);

	return;
      }

      if(recall_audio_run->audio != NULL){
	g_object_unref(G_OBJECT(recall_audio_run->audio));
      }

      if(audio != NULL){
	g_object_ref(G_OBJECT(audio));
      }

      recall_audio_run->audio = audio;

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_RECALL_AUDIO:
    {
      AgsRecallAudio *recall_audio;

      recall_audio = (AgsRecallAudio *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(recall_audio_run->recall_audio == recall_audio){
	g_rec_mutex_unlock(recall_mutex);

	return;
      }

      if(recall_audio_run->recall_audio != NULL){
	g_object_unref(G_OBJECT(recall_audio_run->recall_audio));
      }

      if(recall_audio != NULL){
	g_object_ref(G_OBJECT(recall_audio));
      }

      recall_audio_run->recall_audio = recall_audio;

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_recall_audio_run_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec)
{
  AgsRecallAudioRun *recall_audio_run;

  GRecMutex *recall_mutex;

  recall_audio_run = AGS_RECALL_AUDIO_RUN(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall_audio_run);

  switch(prop_id){
  case PROP_AUDIO:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, recall_audio_run->audio);

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_RECALL_AUDIO:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, recall_audio_run->recall_audio);

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_recall_audio_run_dispose(GObject *gobject)
{
  AgsRecallAudioRun *recall_audio_run;

  recall_audio_run = AGS_RECALL_AUDIO_RUN(gobject);

  /* audio */
  if(recall_audio_run->audio != NULL){
    gpointer tmp;

    tmp = recall_audio_run->audio;

    recall_audio_run->audio = NULL;

    g_object_unref(tmp);
  }

  /* recall audio */
  if(recall_audio_run->recall_audio != NULL){
    gpointer tmp;

    tmp = recall_audio_run->recall_audio;

    recall_audio_run->recall_audio = NULL;

    g_object_unref(tmp);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_recall_audio_run_parent_class)->dispose(gobject);
}

void
ags_recall_audio_run_finalize(GObject *gobject)
{
  AgsRecallAudioRun *recall_audio_run;

  recall_audio_run = AGS_RECALL_AUDIO_RUN(gobject);
  
  /* audio */
  if(recall_audio_run->audio != NULL){
    gpointer tmp;

    tmp = recall_audio_run->audio;

    recall_audio_run->audio = NULL;

    g_object_unref(tmp);
  }

  /* recall audio */
  if(recall_audio_run->recall_audio != NULL){
    gpointer tmp;

    tmp = recall_audio_run->recall_audio;

    recall_audio_run->recall_audio = NULL;

    g_object_unref(tmp);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_recall_audio_run_parent_class)->finalize(gobject);
}

void
ags_recall_audio_run_notify_recall_container_callback(GObject *gobject,
						      GParamSpec *pspec,
						      gpointer user_data)
{
  AgsRecallContainer *recall_container;
  AgsRecallAudioRun *recall_audio_run;
  
  recall_audio_run = AGS_RECALL_AUDIO_RUN(gobject);

  /* get some fields */
  g_object_get(recall_audio_run,
	       "recall-container", &recall_container,
	       NULL);

  if(recall_container != NULL){
    AgsRecallAudio *recall_audio;

    /* recall audio */
    g_object_get(recall_container,
		 "recall-audio", &recall_audio,
		 NULL);

    if(recall_audio != NULL){
      g_object_set(recall_audio_run,
		   "recall-audio", recall_audio,
		   NULL);

      g_object_unref(recall_audio);
    }

    g_object_unref(recall_container);
  }else{
    g_object_set(recall_audio_run,
		 "recall-audio", NULL,
		 NULL);
  }
}

AgsRecall*
ags_recall_audio_run_duplicate(AgsRecall *recall,
			       AgsRecallID *recall_id,
			       guint *n_params, gchar **parameter_name, GValue *value)
{
  AgsAudio *audio;
  AgsRecallAudio *recall_audio;
  AgsRecallAudioRun *recall_audio_run, *copy_recall_audio_run;

  recall_audio_run = AGS_RECALL_AUDIO_RUN(recall);

  /* get some fields */
  audio = NULL;

  recall_audio = NULL;
  
  g_object_get(recall_audio_run,
	       "audio", &audio,
	       "recall-audio", &recall_audio,
	       NULL);

  /* duplicate */
  copy_recall_audio_run = (AgsRecallAudioRun *) AGS_RECALL_CLASS(ags_recall_audio_run_parent_class)->duplicate(recall,
													       recall_id,
													       n_params, parameter_name, value);
  g_object_set(copy_recall_audio_run,
	       "audio", audio,
	       "recall-audio", recall_audio,
	       NULL);

  /* unref */
  if(audio != NULL){
    g_object_unref(audio);
  }

  if(recall_audio != NULL){
    g_object_unref(recall_audio);
  }
  
  return((AgsRecall *) copy_recall_audio_run);
}

/**
 * ags_recall_audio_run_get_audio:
 * @recall_audio_run: the #AgsRecallAudioRun
 * 
 * Get audio.
 * 
 * Returns: (transfer full): the #AgsAudio
 * 
 * Since: 3.1.0
 */
AgsAudio*
ags_recall_audio_run_get_audio(AgsRecallAudioRun *recall_audio_run)
{
  AgsAudio *audio;

  if(!AGS_IS_RECALL_AUDIO_RUN(recall_audio_run)){
    return(NULL);
  }

  g_object_get(recall_audio_run,
	       "audio", &audio,
	       NULL);

  return(audio);
}

/**
 * ags_recall_audio_run_set_audio:
 * @recall_audio_run: the #AgsRecallAudioRun
 * @audio: the #AgsAudio
 * 
 * Set audio.
 * 
 * Since: 3.1.0
 */
void
ags_recall_audio_run_set_audio(AgsRecallAudioRun *recall_audio_run, AgsAudio *audio)
{
  if(!AGS_IS_RECALL_AUDIO_RUN(recall_audio_run)){
    return;
  }

  g_object_set(recall_audio_run,
	       "audio", audio,
	       NULL);
}

/**
 * ags_recall_audio_run_get_recall_audio:
 * @recall_audio_run: the #AgsRecallAudioRun
 * 
 * Get recall audio.
 * 
 * Returns: (transfer full): the #AgsRecallAudio
 * 
 * Since: 3.1.0
 */
AgsRecallAudio*
ags_recall_audio_run_get_recall_audio(AgsRecallAudioRun *recall_audio_run)
{
  AgsRecallAudio *recall_audio;

  if(!AGS_IS_RECALL_AUDIO_RUN(recall_audio_run)){
    return(NULL);
  }

  g_object_get(recall_audio_run,
	       "recall-audio", &recall_audio,
	       NULL);

  return(recall_audio);
}

/**
 * ags_recall_audio_run_set_recall_audio:
 * @recall_audio_run: the #AgsRecallAudioRun
 * @recall_audio: the #AgsRecallAudio
 * 
 * Set recall audio.
 * 
 * Since: 3.1.0
 */
void
ags_recall_audio_run_set_recall_audio(AgsRecallAudioRun *recall_audio_run, AgsRecallAudio *recall_audio)
{
  if(!AGS_IS_RECALL_AUDIO_RUN(recall_audio_run)){
    return;
  }

  g_object_set(recall_audio_run,
	       "recall-audio", recall_audio,
	       NULL);
}

/**
 * ags_recall_audio_run_new:
 *
 * Creates an #AgsRecallAudioRun.
 *
 * Returns: a new #AgsRecallAudioRun.
 *
 * Since: 3.0.0
 */
AgsRecallAudioRun*
ags_recall_audio_run_new()
{
  AgsRecallAudioRun *recall_audio_run;

  recall_audio_run = (AgsRecallAudioRun *) g_object_new(AGS_TYPE_RECALL_AUDIO_RUN,
							NULL);

  return(recall_audio_run);
}
