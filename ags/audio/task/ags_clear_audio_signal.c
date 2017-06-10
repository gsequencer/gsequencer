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

#include <ags/audio/task/ags_clear_audio_signal.h>

#include <ags/object/ags_connectable.h>

#include <math.h>

#include <ags/i18n.h>

void ags_clear_audio_signal_class_init(AgsClearAudioSignalClass *clear_audio_signal);
void ags_clear_audio_signal_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_clear_audio_signal_init(AgsClearAudioSignal *clear_audio_signal);
void ags_clear_audio_signal_set_property(GObject *gobject,
				       guint prop_id,
				       const GValue *value,
				       GParamSpec *param_spec);
void ags_clear_audio_signal_get_property(GObject *gobject,
				       guint prop_id,
				       GValue *value,
				       GParamSpec *param_spec);
void ags_clear_audio_signal_connect(AgsConnectable *connectable);
void ags_clear_audio_signal_disconnect(AgsConnectable *connectable);
void ags_clear_audio_signal_finalize(GObject *gobject);

void ags_clear_audio_signal_launch(AgsTask *task);

/**
 * SECTION:ags_clear_audio_signal
 * @short_description: clear audio_signal object from recycling
 * @title: AgsClearAudioSignal
 * @section_id:
 * @include: ags/audio/task/ags_clear_audio_signal.h
 *
 * The #AgsClearAudioSignal task clears #AgsAudioSignal from #AgsRecycling.
 */

static gpointer ags_clear_audio_signal_parent_class = NULL;
static AgsConnectableInterface *ags_clear_audio_signal_parent_connectable_interface;

enum{
  PROP_0,
  PROP_AUDIO_SIGNAL,
};

GType
ags_clear_audio_signal_get_type()
{
  static GType ags_type_clear_audio_signal = 0;

  if(!ags_type_clear_audio_signal){
    static const GTypeInfo ags_clear_audio_signal_info = {
      sizeof (AgsClearAudioSignalClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_clear_audio_signal_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsClearAudioSignal),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_clear_audio_signal_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_clear_audio_signal_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_clear_audio_signal = g_type_register_static(AGS_TYPE_TASK,
						  "AgsClearAudioSignal",
						  &ags_clear_audio_signal_info,
						  0);

    g_type_add_interface_static(ags_type_clear_audio_signal,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_clear_audio_signal);
}

void
ags_clear_audio_signal_class_init(AgsClearAudioSignalClass *clear_audio_signal)
{
  GObjectClass *gobject;
  AgsTaskClass *task;
  GParamSpec *param_spec;

  ags_clear_audio_signal_parent_class = g_type_class_peek_parent(clear_audio_signal);

  /* GObjectClass */
  gobject = (GObjectClass *) clear_audio_signal;

  gobject->set_property = ags_clear_audio_signal_set_property;
  gobject->get_property = ags_clear_audio_signal_get_property;

  gobject->finalize = ags_clear_audio_signal_finalize;

  /* properties */
  /**
   * AgsClearAudioSignal:audio-signal:
   *
   * The assigned #AgsAudioSignal
   * 
   * Since: 0.7.122.17
   */
  param_spec = g_param_spec_object("audio-signal",
				   i18n_pspec("audio signal of clear audio signal"),
				   i18n_pspec("The audio signal of clear audio signal task"),
				   AGS_TYPE_AUDIO_SIGNAL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO_SIGNAL,
				  param_spec);

  /* AgsTaskClass */
  task = (AgsTaskClass *) clear_audio_signal;

  task->launch = ags_clear_audio_signal_launch;
}

void
ags_clear_audio_signal_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_clear_audio_signal_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_clear_audio_signal_connect;
  connectable->disconnect = ags_clear_audio_signal_disconnect;
}

void
ags_clear_audio_signal_init(AgsClearAudioSignal *clear_audio_signal)
{
  clear_audio_signal->audio_signal = NULL;
}

void
ags_clear_audio_signal_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec)
{
  AgsClearAudioSignal *clear_audio_signal;

  clear_audio_signal = AGS_CLEAR_AUDIO_SIGNAL(gobject);

  switch(prop_id){
  case PROP_AUDIO_SIGNAL:
    {
      AgsAudioSignal *audio_signal;

      audio_signal = (AgsAudioSignal *) g_value_get_object(value);

      if(clear_audio_signal->audio_signal == (GObject *) audio_signal){
	return;
      }

      if(clear_audio_signal->audio_signal != NULL){
	g_object_unref(clear_audio_signal->audio_signal);
      }

      if(audio_signal != NULL){
	g_object_ref(audio_signal);
      }

      clear_audio_signal->audio_signal = (GObject *) audio_signal;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_clear_audio_signal_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec)
{
  AgsClearAudioSignal *clear_audio_signal;

  clear_audio_signal = AGS_CLEAR_AUDIO_SIGNAL(gobject);

  switch(prop_id){
  case PROP_AUDIO_SIGNAL:
    {
      g_value_set_object(value, clear_audio_signal->audio_signal);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_clear_audio_signal_connect(AgsConnectable *connectable)
{
  ags_clear_audio_signal_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_clear_audio_signal_disconnect(AgsConnectable *connectable)
{
  ags_clear_audio_signal_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_clear_audio_signal_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_clear_audio_signal_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_clear_audio_signal_launch(AgsTask *task)
{
  AgsClearAudioSignal *clear_audio_signal;
  
  AgsAudioSignal *audio_signal;

  GList *stream;

  clear_audio_signal = AGS_CLEAR_AUDIO_SIGNAL(task);

  /* clear */
  audio_signal = clear_audio_signal->audio_signal;

  /* clear the stream */
  stream = audio_signal->stream_beginning;
  
  while(stream != NULL){
    ags_audio_buffer_util_clear_buffer(stream->data, 1,
				       audio_signal->buffer_size, ags_audio_buffer_util_format_from_soundcard(audio_signal->format));
    
    stream = stream->next;
  }
}

/**
 * ags_clear_audio_signal_new:
 * @audio_signal: the #AgsAudioSignal to clear
 *
 * Creates an #AgsClearAudioSignal.
 *
 * Returns: an new #AgsClearAudioSignal.
 *
 * Since: 0.7.122.17
 */
AgsClearAudioSignal*
ags_clear_audio_signal_new(AgsAudioSignal *audio_signal)
{
  AgsClearAudioSignal *clear_audio_signal;

  clear_audio_signal = (AgsClearAudioSignal *) g_object_new(AGS_TYPE_CLEAR_AUDIO_SIGNAL,
							      NULL);
  
  clear_audio_signal->audio_signal = audio_signal;

  return(clear_audio_signal);
}
