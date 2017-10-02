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

#include <ags/audio/task/ags_unref_audio_signal.h>

#include <ags/object/ags_connectable.h>

#include <ags/i18n.h>

void ags_unref_audio_signal_class_init(AgsUnrefAudioSignalClass *unref_audio_signal);
void ags_unref_audio_signal_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_unref_audio_signal_init(AgsUnrefAudioSignal *unref_audio_signal);
void ags_unref_audio_signal_set_property(GObject *gobject,
					 guint prop_id,
					 const GValue *value,
					 GParamSpec *param_spec);
void ags_unref_audio_signal_get_property(GObject *gobject,
					 guint prop_id,
					 GValue *value,
					 GParamSpec *param_spec);
void ags_unref_audio_signal_connect(AgsConnectable *connectable);
void ags_unref_audio_signal_disconnect(AgsConnectable *connectable);
void ags_unref_audio_signal_dispose(GObject *gobject);
void ags_unref_audio_signal_finalize(GObject *gobject);

void ags_unref_audio_signal_launch(AgsTask *task);

/**
 * SECTION:ags_unref_audio_signal
 * @short_description: unref audio signal
 * @title: AgsUnrefAudioSignal
 * @section_id:
 * @include: ags/audio/task/ags_unref_audio_signal.h
 *
 * The #AgsUnrefAudioSignal task unrefs #AgsAudioSignal.
 */

enum{
  PROP_0,
  PROP_AUDIO_SIGNAL,
};

static gpointer ags_unref_audio_signal_parent_class = NULL;
static AgsConnectableInterface *ags_unref_audio_signal_parent_connectable_interface;

GType
ags_unref_audio_signal_get_type()
{
  static GType ags_type_unref_audio_signal = 0;

  if(!ags_type_unref_audio_signal){
    static const GTypeInfo ags_unref_audio_signal_info = {
      sizeof (AgsUnrefAudioSignalClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_unref_audio_signal_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsUnrefAudioSignal),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_unref_audio_signal_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_unref_audio_signal_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_unref_audio_signal = g_type_register_static(AGS_TYPE_TASK,
							 "AgsUnrefAudioSignal",
							 &ags_unref_audio_signal_info,
							 0);
    
    g_type_add_interface_static(ags_type_unref_audio_signal,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_unref_audio_signal);
}

void
ags_unref_audio_signal_class_init(AgsUnrefAudioSignalClass *unref_audio_signal)
{
  GObjectClass *gobject;
  AgsTaskClass *task;
  GParamSpec *param_spec;
  
  ags_unref_audio_signal_parent_class = g_type_class_peek_parent(unref_audio_signal);

  /* gobject */
  gobject = (GObjectClass *) unref_audio_signal;

  gobject->set_property = ags_unref_audio_signal_set_property;
  gobject->get_property = ags_unref_audio_signal_get_property;

  gobject->dispose = ags_unref_audio_signal_dispose;
  gobject->finalize = ags_unref_audio_signal_finalize;

  /* properties */  
  /**
   * AgsUnrefAudioSignal:audio-signal:
   *
   * The assigned #AgsAudioSignal
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_object("audio-signal",
				   i18n_pspec("audio signal of unref audio signal"),
				   i18n_pspec("The audio signal of unref audio signal task"),
				   AGS_TYPE_AUDIO_SIGNAL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO_SIGNAL,
				  param_spec);

  /* task */
  task = (AgsTaskClass *) unref_audio_signal;

  task->launch = ags_unref_audio_signal_launch;
}

void
ags_unref_audio_signal_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_unref_audio_signal_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_unref_audio_signal_connect;
  connectable->disconnect = ags_unref_audio_signal_disconnect;
}

void
ags_unref_audio_signal_init(AgsUnrefAudioSignal *unref_audio_signal)
{
  unref_audio_signal->audio_signal = NULL;
}

void
ags_unref_audio_signal_set_property(GObject *gobject,
				    guint prop_id,
				    const GValue *value,
				    GParamSpec *param_spec)
{
  AgsUnrefAudioSignal *unref_audio_signal;

  unref_audio_signal = AGS_UNREF_AUDIO_SIGNAL(gobject);

  switch(prop_id){
  case PROP_AUDIO_SIGNAL:
    {
      AgsAudioSignal *audio_signal;

      audio_signal = (AgsAudioSignal *) g_value_get_object(value);

      if(unref_audio_signal->audio_signal == (GObject *) audio_signal){
	return;
      }

      if(unref_audio_signal->audio_signal != NULL){
	g_object_unref(unref_audio_signal->audio_signal);
      }

      if(audio_signal != NULL){
	g_object_ref(audio_signal);
      }

      unref_audio_signal->audio_signal = (GObject *) audio_signal;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_unref_audio_signal_get_property(GObject *gobject,
				    guint prop_id,
				    GValue *value,
				    GParamSpec *param_spec)
{
  AgsUnrefAudioSignal *unref_audio_signal;

  unref_audio_signal = AGS_UNREF_AUDIO_SIGNAL(gobject);

  switch(prop_id){
  case PROP_AUDIO_SIGNAL:
    {
      g_value_set_object(value, unref_audio_signal->audio_signal);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_unref_audio_signal_connect(AgsConnectable *connectable)
{
  ags_unref_audio_signal_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_unref_audio_signal_disconnect(AgsConnectable *connectable)
{
  ags_unref_audio_signal_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_unref_audio_signal_dispose(GObject *gobject)
{
  AgsUnrefAudioSignal *unref_audio_signal;

  unref_audio_signal = AGS_UNREF_AUDIO_SIGNAL(gobject);

  if(unref_audio_signal->audio_signal != NULL){
    g_object_unref(unref_audio_signal->audio_signal);

    unref_audio_signal->audio_signal = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_unref_audio_signal_parent_class)->dispose(gobject);
}

void
ags_unref_audio_signal_finalize(GObject *gobject)
{
  AgsUnrefAudioSignal *unref_audio_signal;

  unref_audio_signal = AGS_UNREF_AUDIO_SIGNAL(gobject);

  if(unref_audio_signal->audio_signal != NULL){
    g_object_unref(unref_audio_signal->audio_signal);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_unref_audio_signal_parent_class)->finalize(gobject);
}

void
ags_unref_audio_signal_launch(AgsTask *task)
{
  AgsUnrefAudioSignal *unref_audio_signal;

  unref_audio_signal = AGS_UNREF_AUDIO_SIGNAL(task);

  /* cancel AgsRecall */
  g_object_unref(unref_audio_signal->audio_signal);
}

AgsUnrefAudioSignal*
ags_unref_audio_signal_new(AgsAudioSignal *audio_signal)
{
  AgsUnrefAudioSignal *unref_audio_signal;

  unref_audio_signal = (AgsUnrefAudioSignal *) g_object_new(AGS_TYPE_UNREF_AUDIO_SIGNAL,
							    "audio-signal", audio_signal,
							    NULL);

  return(unref_audio_signal);
}
