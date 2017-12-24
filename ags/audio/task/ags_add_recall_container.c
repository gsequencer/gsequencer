/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#include <ags/audio/task/ags_add_recall_container.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>

#include <math.h>

#include <ags/i18n.h>

void ags_add_recall_container_class_init(AgsAddRecallContainerClass *add_recall_container);
void ags_add_recall_container_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_add_recall_container_init(AgsAddRecallContainer *add_recall_container);
void ags_add_recall_container_set_property(GObject *gobject,
					   guint prop_id,
					   const GValue *value,
					   GParamSpec *param_spec);
void ags_add_recall_container_get_property(GObject *gobject,
					   guint prop_id,
					   GValue *value,
					   GParamSpec *param_spec);
void ags_add_recall_container_connect(AgsConnectable *connectable);
void ags_add_recall_container_disconnect(AgsConnectable *connectable);
void ags_add_recall_container_dispose(GObject *gobject);
void ags_add_recall_container_finalize(GObject *gobject);

void ags_add_recall_container_launch(AgsTask *task);

/**
 * SECTION:ags_add_recall_container
 * @short_description: add recall object to context
 * @title: AgsAddRecallContainer
 * @section_id:
 * @include: ags/audio/task/ags_add_recall_container.h
 *
 * The #AgsAddRecallContainer task adds #AgsRecallContainer of audio.
 */

static gpointer ags_add_recall_container_parent_class = NULL;
static AgsConnectableInterface *ags_add_recall_container_parent_connectable_interface;

enum{
  PROP_0,
  PROP_AUDIO,
  PROP_RECALL_CONTAINER,
};

GType
ags_add_recall_container_get_type()
{
  static GType ags_type_add_recall_container = 0;

  if(!ags_type_add_recall_container){
    static const GTypeInfo ags_add_recall_container_info = {
      sizeof (AgsAddRecallContainerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_add_recall_container_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsAddRecallContainer),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_add_recall_container_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_add_recall_container_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_add_recall_container = g_type_register_static(AGS_TYPE_TASK,
							   "AgsAddRecallContainer",
							   &ags_add_recall_container_info,
							   0);

    g_type_add_interface_static(ags_type_add_recall_container,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_add_recall_container);
}

void
ags_add_recall_container_class_init(AgsAddRecallContainerClass *add_recall_container)
{
  GObjectClass *gobject;
  AgsTaskClass *task;
  GParamSpec *param_spec;

  ags_add_recall_container_parent_class = g_type_class_peek_parent(add_recall_container);

  /* GObjectClass */
  gobject = (GObjectClass *) add_recall_container;

  gobject->set_property = ags_add_recall_container_set_property;
  gobject->get_property = ags_add_recall_container_get_property;

  gobject->finalize = ags_add_recall_container_finalize;

  /* properties */
  /**
   * AgsAddRecallContainer:audio:
   *
   * The assigned #AgsAudio
   * 
<<<<<<< HEAD
   * Since: 1.0.0
=======
   * Since: 1.0.0
>>>>>>> master
   */
  param_spec = g_param_spec_object("audio",
				   i18n_pspec("audio of add recall container"),
				   i18n_pspec("The audio of add recall container task"),
				   AGS_TYPE_AUDIO,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO,
				  param_spec);
  
  /**
   * AgsAddRecallContainer:recall_container:
   *
   * The assigned #AgsRecall_Container
   * 
<<<<<<< HEAD
   * Since: 1.0.0
=======
   * Since: 1.0.0
>>>>>>> master
   */
  param_spec = g_param_spec_object("recall-container",
				   i18n_pspec("recall container of add recall container"),
				   i18n_pspec("The recall container of add recall container task"),
				   AGS_TYPE_RECALL_CONTAINER,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RECALL_CONTAINER,
				  param_spec);

  /* AgsTaskClass */
  task = (AgsTaskClass *) add_recall_container;

  task->launch = ags_add_recall_container_launch;
}

void
ags_add_recall_container_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_add_recall_container_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_add_recall_container_connect;
  connectable->disconnect = ags_add_recall_container_disconnect;
}

void
ags_add_recall_container_init(AgsAddRecallContainer *add_recall_container)
{
  add_recall_container->audio = NULL;
  add_recall_container->recall_container = NULL;
}

void
ags_add_recall_container_set_property(GObject *gobject,
				      guint prop_id,
				      const GValue *value,
				      GParamSpec *param_spec)
{
  AgsAddRecallContainer *add_recall_container;

  add_recall_container = AGS_ADD_RECALL_CONTAINER(gobject);

  switch(prop_id){
  case PROP_AUDIO:
    {
      AgsAudio *audio;

      audio = (AgsAudio *) g_value_get_object(value);

      if(add_recall_container->audio == (GObject *) audio){
	return;
      }

      if(add_recall_container->audio != NULL){
	g_object_unref(add_recall_container->audio);
      }

      if(audio != NULL){
	g_object_ref(audio);
      }

      add_recall_container->audio = (GObject *) audio;
    }
    break;
  case PROP_RECALL_CONTAINER:
    {
      AgsRecallContainer *recall_container;

      recall_container = (AgsRecallContainer *) g_value_get_object(value);

      if(add_recall_container->recall_container == (GObject *) recall_container){
	return;
      }

      if(add_recall_container->recall_container != NULL){
	g_object_unref(add_recall_container->recall_container);
      }

      if(recall_container != NULL){
	g_object_ref(recall_container);
      }

      add_recall_container->recall_container = (GObject *) recall_container;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_add_recall_container_get_property(GObject *gobject,
				      guint prop_id,
				      GValue *value,
				      GParamSpec *param_spec)
{
  AgsAddRecallContainer *add_recall_container;

  add_recall_container = AGS_ADD_RECALL_CONTAINER(gobject);

  switch(prop_id){
  case PROP_AUDIO:
    {
      g_value_set_object(value, add_recall_container->audio);
    }
    break;
  case PROP_RECALL_CONTAINER:
    {
      g_value_set_object(value, add_recall_container->recall_container);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_add_recall_container_connect(AgsConnectable *connectable)
{
  ags_add_recall_container_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_add_recall_container_disconnect(AgsConnectable *connectable)
{
  ags_add_recall_container_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_add_recall_container_dispose(GObject *gobject)
{
  AgsAddRecallContainer *add_recall_container;

  add_recall_container = AGS_ADD_RECALL_CONTAINER(gobject);

  if(add_recall_container->audio != NULL){
    g_object_unref(add_recall_container->audio);

    add_recall_container->audio = NULL;
  }

  if(add_recall_container->recall_container != NULL){
    g_object_unref(add_recall_container->recall_container);

    add_recall_container->recall_container = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_add_recall_container_parent_class)->dispose(gobject);
}

void
ags_add_recall_container_finalize(GObject *gobject)
{
  AgsAddRecallContainer *add_recall_container;

  add_recall_container = AGS_ADD_RECALL_CONTAINER(gobject);

  if(add_recall_container->audio != NULL){
    g_object_unref(add_recall_container->audio);
  }

  if(add_recall_container->recall_container != NULL){
    g_object_unref(add_recall_container->recall_container);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_add_recall_container_parent_class)->finalize(gobject);
}

void
ags_add_recall_container_launch(AgsTask *task)
{
  AgsAddRecallContainer *add_recall_container;

  add_recall_container = AGS_ADD_RECALL_CONTAINER(task);

  ags_audio_add_recall_container((AgsAudio *) add_recall_container->audio,
				 (GObject *) add_recall_container->recall_container);
}

/**
 * ags_add_recall_container_new:
 * @audio: the #AgsAudio
 * @recall_container: the #AgsRecallContainer to add
 *
 * Creates an #AgsAddRecallContainer.
 *
 * Returns: an new #AgsAddRecallContainer.
 *
 * Since: 1.0.0
 */
AgsAddRecallContainer*
ags_add_recall_container_new(GObject *audio,
			     AgsRecallContainer *recall_container)
{
  AgsAddRecallContainer *add_recall_container;

  add_recall_container = (AgsAddRecallContainer *) g_object_new(AGS_TYPE_ADD_RECALL_CONTAINER,
								"audio", audio,
								"recall-container", recall_container,
								NULL);  

  return(add_recall_container);
}
