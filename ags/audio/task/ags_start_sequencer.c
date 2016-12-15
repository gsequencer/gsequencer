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

#include <ags/audio/task/ags_start_sequencer.h>

#include <ags/object/ags_connectable.h>

#include <ags/audio/thread/ags_audio_loop.h>
#include <ags/audio/thread/ags_sequencer_thread.h>

void ags_start_sequencer_class_init(AgsStartSequencerClass *start_sequencer);
void ags_start_sequencer_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_start_sequencer_init(AgsStartSequencer *start_sequencer);
void ags_start_sequencer_set_property(GObject *gobject,
				      guint prop_id,
				      const GValue *value,
				      GParamSpec *param_spec);
void ags_start_sequencer_get_property(GObject *gobject,
				      guint prop_id,
				      GValue *value,
				      GParamSpec *param_spec);
void ags_start_sequencer_connect(AgsConnectable *connectable);
void ags_start_sequencer_disconnect(AgsConnectable *connectable);
void ags_start_sequencer_finalize(GObject *gobject);

void ags_start_sequencer_launch(AgsTask *task);

/**
 * SECTION:ags_start_sequencer
 * @short_description: start sequencer object
 * @title: AgsStartSequencer
 * @section_id:
 * @include: ags/audio/task/ags_start_sequencer.h
 *
 * The #AgsStartSequencer task starts sequencer.
 */

static gpointer ags_start_sequencer_parent_class = NULL;
static AgsConnectableInterface *ags_start_sequencer_parent_connectable_interface;

enum{
  PROP_0,
  PROP_APPLICATION_CONTEXT,
};

GType
ags_start_sequencer_get_type()
{
  static GType ags_type_start_sequencer = 0;

  if(!ags_type_start_sequencer){
    static const GTypeInfo ags_start_sequencer_info = {
      sizeof (AgsStartSequencerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_start_sequencer_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsStartSequencer),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_start_sequencer_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_start_sequencer_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_start_sequencer = g_type_register_static(AGS_TYPE_TASK,
						      "AgsStartSequencer\0",
						      &ags_start_sequencer_info,
						      0);

    g_type_add_interface_static(ags_type_start_sequencer,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_start_sequencer);
}

void
ags_start_sequencer_class_init(AgsStartSequencerClass *start_sequencer)
{
  GObjectClass *gobject;
  AgsTaskClass *task;
  GParamSpec *param_spec;

  ags_start_sequencer_parent_class = g_type_class_peek_parent(start_sequencer);

  /* gobject */
  gobject = (GObjectClass *) start_sequencer;

  gobject->set_property = ags_start_sequencer_set_property;
  gobject->get_property = ags_start_sequencer_get_property;

  gobject->finalize = ags_start_sequencer_finalize;

  /* properties */
  /**
   * AgsStartSequencer:application-context:
   *
   * The assigned #AgsApplicationContext
   * 
   * Since: 0.7.117
   */
  param_spec = g_param_spec_object("application-context\0",
				   "application context of start sequencer\0",
				   "The application context of start sequencer task\0",
				   AGS_TYPE_APPLICATION_CONTEXT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_APPLICATION_CONTEXT,
				  param_spec);

  /* task */
  task = (AgsTaskClass *) start_sequencer;

  task->launch = ags_start_sequencer_launch;
}

void
ags_start_sequencer_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_start_sequencer_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_start_sequencer_connect;
  connectable->disconnect = ags_start_sequencer_disconnect;
}

void
ags_start_sequencer_init(AgsStartSequencer *start_sequencer)
{
  start_sequencer->application_context = NULL;
}

void
ags_start_sequencer_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec)
{
  AgsStartSequencer *start_sequencer;

  start_sequencer = AGS_START_SEQUENCER(gobject);

  switch(prop_id){
  case PROP_APPLICATION_CONTEXT:
    {
      GObject *application_context;

      application_context = (GObject *) g_value_get_object(value);

      if(start_sequencer->application_context == (GObject *) application_context){
	return;
      }

      if(start_sequencer->application_context != NULL){
	g_object_unref(start_sequencer->application_context);
      }

      if(application_context != NULL){
	g_object_ref(application_context);
      }

      start_sequencer->application_context = (GObject *) application_context;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_start_sequencer_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec)
{
  AgsStartSequencer *start_sequencer;

  start_sequencer = AGS_START_SEQUENCER(gobject);

  switch(prop_id){
  case PROP_APPLICATION_CONTEXT:
    {
      g_value_set_object(value, start_sequencer->application_context);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_start_sequencer_connect(AgsConnectable *connectable)
{
  ags_start_sequencer_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_start_sequencer_disconnect(AgsConnectable *connectable)
{
  ags_start_sequencer_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_start_sequencer_finalize(GObject *gobject)
{
  AgsAudioLoop *audio_loop;
  AgsSequencerThread *sequencer_thread;

  AgsApplicationContext *application_context;
  AgsSequencer *sequencer;

  application_context = AGS_START_SEQUENCER(gobject)->application_context;
  audio_loop = AGS_AUDIO_LOOP(application_context->main_loop);

  sequencer_thread = (AgsSequencerThread *) ags_thread_find_type((AgsThread *) audio_loop,
								 AGS_TYPE_SEQUENCER_THREAD);

  if(sequencer_thread->error != NULL){
    g_error_free(sequencer_thread->error);

    sequencer_thread->error = NULL;
  }

  G_OBJECT_CLASS(ags_start_sequencer_parent_class)->finalize(gobject);
}

void
ags_start_sequencer_launch(AgsTask *task)
{
  AgsStartSequencer *start_sequencer;

  AgsAudioLoop *audio_loop;
  AgsThread *sequencer_thread;

  AgsApplicationContext *application_context;

  GList *start_queue;
  
  start_sequencer = AGS_START_SEQUENCER(task);

  application_context = start_sequencer->application_context;
  audio_loop = AGS_AUDIO_LOOP(application_context->main_loop);

  /*
  if(ags_sequencer_is_starting(sequencer) ||
     ags_sequencer_is_playing(sequencer)){
    return;
  }
  */
  sequencer_thread = audio_loop;
  
  while((sequencer_thread = ags_thread_find_type(sequencer_thread,
						 AGS_TYPE_SEQUENCER_THREAD)) != NULL){
    /* append to AgsSequencer */
    AGS_SEQUENCER_THREAD(sequencer_thread)->error = NULL;

    g_message("start sequencer\0");

    start_queue = NULL;    
    start_queue = g_list_prepend(start_queue,
				 sequencer_thread);

    if(start_queue != NULL){
      if(g_atomic_pointer_get(&(AGS_THREAD(audio_loop)->start_queue)) != NULL){
	g_atomic_pointer_set(&(AGS_THREAD(audio_loop)->start_queue),
			     g_list_concat(start_queue,
					   g_atomic_pointer_get(&(AGS_THREAD(audio_loop)->start_queue))));
      }else{
	g_atomic_pointer_set(&(AGS_THREAD(audio_loop)->start_queue),
			     start_queue);
      }
    }

    sequencer_thread = g_atomic_pointer_get(&(sequencer_thread->next));
  }
}

/**
 * ags_start_sequencer_new:
 * @application_context: the #AgsApplicationContext
 *
 * Creates an #AgsStartSequencer.
 *
 * Returns: an new #AgsStartSequencer.
 *
 * Since: 0.7.117
 */
AgsStartSequencer*
ags_start_sequencer_new(AgsApplicationContext *application_context)
{
  AgsStartSequencer *start_sequencer;

  start_sequencer = (AgsStartSequencer *) g_object_new(AGS_TYPE_START_SEQUENCER,
						       NULL);

  start_sequencer->application_context = application_context;

  return(start_sequencer);
}
