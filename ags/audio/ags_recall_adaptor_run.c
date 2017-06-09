/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#include <ags/audio/ags_recall_adaptor_run.h>

#include <ags/util/ags_id_generator.h>

#include <ags/object/ags_config.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_dynamic_connectable.h>

#include <ags/thread/ags_mutex_manager.h>

#include <ags/file/ags_file_stock.h>
#include <ags/file/ags_file_id_ref.h>
#include <ags/file/ags_file_lookup.h>

void ags_recall_adaptor_run_class_init(AgsRecallAdaptorRunClass *recall_adaptor_run);
void ags_recall_adaptor_run_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_recall_adaptor_run_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable);
void ags_recall_adaptor_run_init(AgsRecallAdaptorRun *recall_adaptor_run);
void ags_recall_adaptor_run_set_property(GObject *gobject,
					 guint prop_id,
					 const GValue *value,
					 GParamSpec *param_spec);
void ags_recall_adaptor_run_get_property(GObject *gobject,
					 guint prop_id,
					 GValue *value,
					 GParamSpec *param_spec);
void ags_recall_adaptor_run_finalize(GObject *gobject);
void ags_recall_adaptor_run_connect(AgsConnectable *connectable);
void ags_recall_adaptor_run_disconnect(AgsConnectable *connectable);
void ags_recall_adaptor_run_connect_dynamic(AgsDynamicConnectable *dynamic_connectable);
void ags_recall_adaptor_run_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable); 

AgsRecall* ags_recall_adaptor_run_duplicate(AgsRecall *recall,
					    AgsRecallID *recall_id,
					    guint *n_params, GParameter *parameter);
void ags_recall_adaptor_run_run_inter(AgsRecall *recall);

/**
 * SECTION:ags_recall_adaptor_run
 * @short_description: recall adaptor
 * @title: AgsRecallAdaptorRun
 * @section_id:
 * @include: ags/audio/recall/ags_recall_adaptor_run.h
 *
 * The #AgsRecallAdaptorRun acts as an adaptor of different audio
 * mapping. It is used to divide out non-/interleaved multi channel
 * audio buffers.
 */

static gpointer ags_recall_adaptor_run_parent_class = NULL;
static AgsConnectableInterface* ags_recall_adaptor_run_parent_connectable_interface;
static AgsDynamicConnectableInterface *ags_recall_adaptor_run_parent_dynamic_connectable_interface;

GType
ags_recall_adaptor_run_get_type()
{
  static GType ags_type_recall_adaptor_run = 0;

  if(!ags_type_recall_adaptor_run){
    static const GTypeInfo ags_recall_adaptor_run_info = {
      sizeof (AgsRecallAdaptorRunClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_recall_adaptor_run_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRecallAdaptorRun),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_recall_adaptor_run_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_recall_adaptor_run_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_dynamic_connectable_interface_info = {
      (GInterfaceInitFunc) ags_recall_adaptor_run_dynamic_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_recall_adaptor_run = g_type_register_static(AGS_TYPE_RECALL_AUDIO_RUN,
							 "AgsRecallAdaptorRun",
							 &ags_recall_adaptor_run_info,
							 0);

    g_type_add_interface_static(ags_type_recall_adaptor_run,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_recall_adaptor_run,
				AGS_TYPE_DYNAMIC_CONNECTABLE,
				&ags_dynamic_connectable_interface_info);
  }

  return(ags_type_recall_adaptor_run);
}

void
ags_recall_adaptor_run_class_init(AgsRecallAdaptorRunClass *recall_adaptor_run)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  GParamSpec *param_spec;

  ags_recall_adaptor_run_parent_class = g_type_class_peek_parent(recall_adaptor_run);

  /* GObjectClass */
  gobject = (GObjectClass *) recall_adaptor_run;

  gobject->set_property = ags_recall_adaptor_run_set_property;
  gobject->get_property = ags_recall_adaptor_run_get_property;

  gobject->finalize = ags_recall_adaptor_run_finalize;

  /* properties */

  /* AgsRecallClass */
  recall = (AgsRecallClass *) recall_adaptor_run;

  recall->run_inter = ags_recall_adaptor_run_run_inter;
}

void
ags_recall_adaptor_run_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_recall_adaptor_run_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_recall_adaptor_run_connect;
  connectable->disconnect = ags_recall_adaptor_run_disconnect;
}

void
ags_recall_adaptor_run_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable)
{
  ags_recall_adaptor_run_parent_dynamic_connectable_interface = g_type_interface_peek_parent(dynamic_connectable);

  dynamic_connectable->connect_dynamic = ags_recall_adaptor_run_connect_dynamic;
  dynamic_connectable->disconnect_dynamic = ags_recall_adaptor_run_disconnect_dynamic;
}

void
ags_recall_adaptor_run_init(AgsRecallAdaptorRun *recall_adaptor_run)
{
}

void
ags_recall_adaptor_run_set_property(GObject *gobject,
				    guint prop_id,
				    const GValue *value,
				    GParamSpec *param_spec)
{
  AgsRecallAdaptorRun *recall_adaptor_run;

  recall_adaptor_run = AGS_RECALL_ADAPTOR_RUN(gobject);

  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_recall_adaptor_run_get_property(GObject *gobject,
				    guint prop_id,
				    GValue *value,
				    GParamSpec *param_spec)
{
  AgsRecallAdaptorRun *recall_adaptor_run;
  
  recall_adaptor_run = AGS_RECALL_ADAPTOR_RUN(gobject);

  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_recall_adaptor_run_finalize(GObject *gobject)
{
  AgsRecallAdaptorRun *recall_adaptor_run;

  recall_adaptor_run = AGS_RECALL_ADAPTOR_RUN(gobject);

  G_OBJECT_CLASS(ags_recall_adaptor_run_parent_class)->finalize(gobject);
}

void
ags_recall_adaptor_run_connect(AgsConnectable *connectable)
{
  AgsRecallAdaptorRun *recall_adaptor_run;

  if((AGS_RECALL_CONNECTED & (AGS_RECALL(connectable)->flags)) != 0){
    return;
  }

  /* call parent */
  ags_recall_adaptor_run_parent_connectable_interface->connect(connectable);

  recall_adaptor_run = AGS_RECALL_ADAPTOR_RUN(connectable);
}

void
ags_recall_adaptor_run_disconnect(AgsConnectable *connectable)
{
  /* call parent */
  ags_recall_adaptor_run_parent_connectable_interface->disconnect(connectable);
}

void
ags_recall_adaptor_run_connect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  AgsRecallAdaptorRun *recall_adaptor_run;

  if((AGS_RECALL_DYNAMIC_CONNECTED & (AGS_RECALL(dynamic_connectable)->flags)) != 0){
    return;
  }

  recall_adaptor_run = AGS_RECALL_ADAPTOR_RUN(dynamic_connectable);

  /* call parent */
  ags_recall_adaptor_run_parent_dynamic_connectable_interface->connect_dynamic(dynamic_connectable);  
}

void
ags_recall_adaptor_run_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  AgsRecallAdaptorRun *recall_adaptor_run;

  /* call parent */
  ags_recall_adaptor_run_parent_dynamic_connectable_interface->disconnect_dynamic(dynamic_connectable);

  recall_adaptor_run = AGS_RECALL_ADAPTOR_RUN(dynamic_connectable);
}

AgsRecall*
ags_recall_adaptor_run_duplicate(AgsRecall *recall,
				 AgsRecallID *recall_id,
				 guint *n_params, GParameter *parameter)
{
  AgsRecallAudio *recall_audio;
  AgsRecallAdaptorRun *copy;

  AgsMutexManager *mutex_manager;

  guint buffer_size;
  
  pthread_mutex_t *application_mutex;
  pthread_mutex_t *audio_mutex;

  /*  */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* get recall audio */
  recall_audio = AGS_RECALL_AUDIO_RUN(recall)->recall_audio;

  /* duplicate */
  copy = AGS_RECALL_ADAPTOR_RUN(AGS_RECALL_CLASS(ags_recall_adaptor_run_parent_class)->duplicate(recall,
												 recall_id,
												 n_params, parameter));

  /* audio mutex */
  pthread_mutex_lock(application_mutex);

  audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					 recall_audio->audio);

  pthread_mutex_unlock(application_mutex);

  /* get buffer size */
  pthread_mutex_lock(audio_mutex);
  
  buffer_size = recall_audio->audio->buffer_size;

  pthread_mutex_unlock(audio_mutex);
  
  /* allocate multi-channel buffer */
  copy->buffer = malloc(recall_audio->n_channels * buffer_size);
  memset(copy->buffer, 0, recall_audio->n_channels * buffer_size);
  
  return((AgsRecall *) copy);
}

void
ags_recall_adaptor_run_run_inter(AgsRecall *recall)
{
  AgsAudio *audio;
  AgsChannel *start, *current;
  AgsRecallAudio *recall_audio;
  AgsRecallAdaptorRun *recall_adaptor_run;
  
  AgsMutexManager *mutex_manager;

  guint samplerate;
  guint buffer_size;
  guint format;

  guint audio_channels;

  guint i;
  
  pthread_mutex_t *application_mutex;
  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *channel_mutex;

  /*  */
  recall_adaptor_run = (AgsRecallAdaptorRun *) recall;
  
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* get recall audio */
  recall_audio = AGS_RECALL_AUDIO_RUN(recall)->recall_audio;
  audio = recall_audio->audio;
  
  /* get audio mutex */
  pthread_mutex_lock(application_mutex);

  audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					 audio);

  pthread_mutex_unlock(application_mutex);

  /* get buffer size */
  pthread_mutex_lock(audio_mutex);

  start = audio->input;

  samplerate = audio->samplerate;
  buffer_size = audio->buffer_size;
  format = audio->format;

  audio_channels = audio->audio_channels;
  
  pthread_mutex_unlock(audio_mutex);
  
  /* fill the adapted buffers */
  for(i = 0; i < audio_channels; i++){
    current = ags_channel_nth(start, recall_audio->mapping[i]);

    /* get channel mutex */
    pthread_mutex_lock(application_mutex);

    channel_mutex = ags_mutex_manager_lookup(mutex_manager,
					     current);

    pthread_mutex_unlock(application_mutex);

    if(current != NULL){
      pthread_mutex_lock(channel_mutex);
      pthread_mutex_unlock(channel_mutex);
      
      //TODO:JK: implement me
      //      ags_audio_buffer_util_copy_buffer_to_buffer(,);
    }
  }

  /* call parent */
  AGS_RECALL_CLASS(ags_recall_adaptor_run_parent_class)->run_inter(recall);
}

/**
 * ags_recall_adaptor_run_new:
 *
 * Creates an #AgsRecallAdaptorRun
 *
 * Returns: a new #AgsRecallAdaptorRun
 *
 * Since: 0.7.128
 */
AgsRecallAdaptorRun*
ags_recall_adaptor_run_new()
{
  AgsRecallAdaptorRun *recall_adaptor_run;

  recall_adaptor_run = (AgsRecallAdaptorRun *) g_object_new(AGS_TYPE_RECALL_ADAPTOR_RUN,
							    NULL);

  return(recall_adaptor_run);
}
