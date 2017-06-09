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

#include <ags/audio/task/ags_remove_audio_signal.h>

#include <ags/object/ags_connectable.h>

#include <math.h>

void ags_remove_audio_signal_class_init(AgsRemoveAudioSignalClass *remove_audio_signal);
void ags_remove_audio_signal_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_remove_audio_signal_init(AgsRemoveAudioSignal *remove_audio_signal);
void ags_remove_audio_signal_set_property(GObject *gobject,
				       guint prop_id,
				       const GValue *value,
				       GParamSpec *param_spec);
void ags_remove_audio_signal_get_property(GObject *gobject,
				       guint prop_id,
				       GValue *value,
				       GParamSpec *param_spec);
void ags_remove_audio_signal_connect(AgsConnectable *connectable);
void ags_remove_audio_signal_disconnect(AgsConnectable *connectable);
void ags_remove_audio_signal_finalize(GObject *gobject);

void ags_remove_audio_signal_launch(AgsTask *task);

/**
 * SECTION:ags_remove_audio_signal
 * @short_description: remove audio_signal object from recycling
 * @title: AgsRemoveAudioSignal
 * @section_id:
 * @include: ags/audio/task/ags_remove_audio_signal.h
 *
 * The #AgsRemoveAudioSignal task removes #AgsAudioSignal from #AgsRecycling.
 */

static gpointer ags_remove_audio_signal_parent_class = NULL;
static AgsConnectableInterface *ags_remove_audio_signal_parent_connectable_interface;

enum{
  PROP_0,
  PROP_RECYCLING,
  PROP_AUDIO_SIGNAL,
};

GType
ags_remove_audio_signal_get_type()
{
  static GType ags_type_remove_audio_signal = 0;

  if(!ags_type_remove_audio_signal){
    static const GTypeInfo ags_remove_audio_signal_info = {
      sizeof (AgsRemoveAudioSignalClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_remove_audio_signal_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRemoveAudioSignal),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_remove_audio_signal_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_remove_audio_signal_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_remove_audio_signal = g_type_register_static(AGS_TYPE_TASK,
						  "AgsRemoveAudioSignal",
						  &ags_remove_audio_signal_info,
						  0);

    g_type_add_interface_static(ags_type_remove_audio_signal,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_remove_audio_signal);
}

void
ags_remove_audio_signal_class_init(AgsRemoveAudioSignalClass *remove_audio_signal)
{
  GObjectClass *gobject;
  AgsTaskClass *task;
  GParamSpec *param_spec;

  ags_remove_audio_signal_parent_class = g_type_class_peek_parent(remove_audio_signal);

  /* GObjectClass */
  gobject = (GObjectClass *) remove_audio_signal;

  gobject->set_property = ags_remove_audio_signal_set_property;
  gobject->get_property = ags_remove_audio_signal_get_property;

  gobject->finalize = ags_remove_audio_signal_finalize;

  /* properties */
  /**
   * AgsRemoveAudioSignal:recycling:
   *
   * The assigned #AgsRecycling
   * 
   * Since: 0.7.117
   */
  param_spec = g_param_spec_object("recycling",
				   "recycling of remove audio signal",
				   "The recycling of remove audio signal task",
		 		   AGS_TYPE_RECYCLING,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RECYCLING,
				  param_spec);

  /**
   * AgsRemoveAudioSignal:audio-signal:
   *
   * The assigned #AgsAudioSignal
   * 
   * Since: 0.7.117
   */
  param_spec = g_param_spec_object("audio-signal",
				   "audio signal of remove audio signal",
				   "The audio signal of remove audio signal task",
				   AGS_TYPE_AUDIO_SIGNAL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO_SIGNAL,
				  param_spec);

  /* AgsTaskClass */
  task = (AgsTaskClass *) remove_audio_signal;

  task->launch = ags_remove_audio_signal_launch;
}

void
ags_remove_audio_signal_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_remove_audio_signal_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_remove_audio_signal_connect;
  connectable->disconnect = ags_remove_audio_signal_disconnect;
}

void
ags_remove_audio_signal_init(AgsRemoveAudioSignal *remove_audio_signal)
{
  remove_audio_signal->recycling = NULL;
  remove_audio_signal->audio_signal = NULL;
}

void
ags_remove_audio_signal_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec)
{
  AgsRemoveAudioSignal *remove_audio_signal;

  remove_audio_signal = AGS_REMOVE_AUDIO_SIGNAL(gobject);

  switch(prop_id){
  case PROP_RECYCLING:
    {
      AgsRecycling *recycling;

      recycling = (AgsRecycling *) g_value_get_object(value);

      if(remove_audio_signal->recycling == (GObject *) recycling){
	return;
      }

      if(remove_audio_signal->recycling != NULL){
	g_object_unref(remove_audio_signal->recycling);
      }

      if(recycling != NULL){
	g_object_ref(recycling);
      }

      remove_audio_signal->recycling = (GObject *) recycling;
    }
    break;
  case PROP_AUDIO_SIGNAL:
    {
      AgsAudioSignal *audio_signal;

      audio_signal = (AgsAudioSignal *) g_value_get_object(value);

      if(remove_audio_signal->audio_signal == (GObject *) audio_signal){
	return;
      }

      if(remove_audio_signal->audio_signal != NULL){
	g_object_unref(remove_audio_signal->audio_signal);
      }

      if(audio_signal != NULL){
	g_object_ref(audio_signal);
      }

      remove_audio_signal->audio_signal = (GObject *) audio_signal;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_remove_audio_signal_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec)
{
  AgsRemoveAudioSignal *remove_audio_signal;

  remove_audio_signal = AGS_REMOVE_AUDIO_SIGNAL(gobject);

  switch(prop_id){
  case PROP_RECYCLING:
    {
      g_value_set_object(value, remove_audio_signal->recycling);
    }
    break;
  case PROP_AUDIO_SIGNAL:
    {
      g_value_set_object(value, remove_audio_signal->audio_signal);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_remove_audio_signal_connect(AgsConnectable *connectable)
{
  ags_remove_audio_signal_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_remove_audio_signal_disconnect(AgsConnectable *connectable)
{
  ags_remove_audio_signal_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_remove_audio_signal_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_remove_audio_signal_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_remove_audio_signal_launch(AgsTask *task)
{
  AgsRemoveAudioSignal *remove_audio_signal;

  remove_audio_signal = AGS_REMOVE_AUDIO_SIGNAL(task);

  ags_recycling_remove_audio_signal(remove_audio_signal->recycling,
				    remove_audio_signal->audio_signal);
}

/**
 * ags_remove_audio_signal_new:
 * @recycling: the #AgsRecycling
 * @audio_signal: the #AgsAudioSignal to remove
 *
 * Creates an #AgsRemoveAudioSignal.
 *
 * Returns: an new #AgsRemoveAudioSignal.
 *
 * Since: 0.4
 */
AgsRemoveAudioSignal*
ags_remove_audio_signal_new(AgsRecycling *recycling,
			    AgsAudioSignal *audio_signal)
{
  AgsRemoveAudioSignal *remove_audio_signal;

  remove_audio_signal = (AgsRemoveAudioSignal *) g_object_new(AGS_TYPE_REMOVE_AUDIO_SIGNAL,
							      NULL);
  

  remove_audio_signal->recycling = recycling;
  remove_audio_signal->audio_signal = audio_signal;

  return(remove_audio_signal);
}
