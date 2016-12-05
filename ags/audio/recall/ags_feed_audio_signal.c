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

#include <ags/audio/recall/ags_feed_audio_signal.h>
#include <ags/audio/recall/ags_feed_channel.h>

#include <ags/lib/ags_parameter.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_dynamic_connectable.h>

#include <ags/audio/ags_recall_channel_run.h>

void ags_feed_audio_signal_class_init(AgsFeedAudioSignalClass *feed_audio_signal);
void ags_feed_audio_signal_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_feed_audio_signal_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable);
void ags_feed_audio_signal_init(AgsFeedAudioSignal *feed_audio_signal);
void ags_feed_audio_signal_connect(AgsConnectable *connectable);
void ags_feed_audio_signal_disconnect(AgsConnectable *connectable);
void ags_feed_audio_signal_connect_dynamic(AgsDynamicConnectable *dynamic_connectable);
void ags_feed_audio_signal_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable);
void ags_feed_audio_signal_finalize(GObject *gobject);

void ags_feed_audio_signal_run_pre(AgsRecall *recall);
AgsRecall* ags_feed_audio_signal_duplicate(AgsRecall *recall,
					   AgsRecallID *recall_id,
					   guint *n_params, GParameter *parameter);

/**
 * SECTION:ags_feed_audio_signal
 * @short_description: feeds audio signal
 * @title: AgsFeedAudioSignal
 * @section_id:
 * @include: ags/audio/recall/ags_feed_audio_signal.h
 *
 * The #AgsFeedAudioSignal class feeds the audio signal.
 */

static gpointer ags_feed_audio_signal_parent_class = NULL;
static AgsConnectableInterface *ags_feed_audio_signal_parent_connectable_interface;
static AgsDynamicConnectableInterface *ags_feed_audio_signal_parent_dynamic_connectable_interface;

GType
ags_feed_audio_signal_get_type()
{
  static GType ags_type_feed_audio_signal = 0;

  if(!ags_type_feed_audio_signal){
    static const GTypeInfo ags_feed_audio_signal_info = {
      sizeof (AgsFeedAudioSignalClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_feed_audio_signal_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsFeedAudioSignal),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_feed_audio_signal_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_feed_audio_signal_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_dynamic_connectable_interface_info = {
      (GInterfaceInitFunc) ags_feed_audio_signal_dynamic_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_feed_audio_signal = g_type_register_static(AGS_TYPE_RECALL_AUDIO_SIGNAL,
							"AgsFeedAudioSignal\0",
							&ags_feed_audio_signal_info,
							0);

    g_type_add_interface_static(ags_type_feed_audio_signal,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_feed_audio_signal,
				AGS_TYPE_DYNAMIC_CONNECTABLE,
				&ags_dynamic_connectable_interface_info);
  }

  return (ags_type_feed_audio_signal);
}

void
ags_feed_audio_signal_class_init(AgsFeedAudioSignalClass *feed_audio_signal)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  GParamSpec *param_spec;

  ags_feed_audio_signal_parent_class = g_type_class_peek_parent(feed_audio_signal);

  /* GObjectClass */
  gobject = (GObjectClass *) feed_audio_signal;

  gobject->finalize = ags_feed_audio_signal_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) feed_audio_signal;

  recall->run_pre = ags_feed_audio_signal_run_pre;
}

void
ags_feed_audio_signal_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_feed_audio_signal_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_feed_audio_signal_connect;
  connectable->disconnect = ags_feed_audio_signal_disconnect;
}

void
ags_feed_audio_signal_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable)
{
  ags_feed_audio_signal_parent_dynamic_connectable_interface = g_type_interface_peek_parent(dynamic_connectable);

  dynamic_connectable->connect_dynamic = ags_feed_audio_signal_connect_dynamic;
  dynamic_connectable->disconnect_dynamic = ags_feed_audio_signal_disconnect_dynamic;
}

void
ags_feed_audio_signal_init(AgsFeedAudioSignal *feed_audio_signal)
{
  AGS_RECALL(feed_audio_signal)->name = "ags-feed\0";
  AGS_RECALL(feed_audio_signal)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(feed_audio_signal)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(feed_audio_signal)->xml_type = "ags-feed-audio-signal\0";
  AGS_RECALL(feed_audio_signal)->port = NULL;
}

void
ags_feed_audio_signal_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_feed_audio_signal_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_feed_audio_signal_connect(AgsConnectable *connectable)
{
  /* call parent */
  ags_feed_audio_signal_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_feed_audio_signal_disconnect(AgsConnectable *connectable)
{
  /* call parent */
  ags_feed_audio_signal_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_feed_audio_signal_connect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  /* call parent */
  ags_feed_audio_signal_parent_dynamic_connectable_interface->connect_dynamic(dynamic_connectable);

  /* empty */
}

void
ags_feed_audio_signal_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  /* call parent */
  ags_feed_audio_signal_parent_dynamic_connectable_interface->disconnect_dynamic(dynamic_connectable);

  /* empty */
}

void
ags_feed_audio_signal_run_pre(AgsRecall *recall)
{
  AgsAudioSignal *template, *audio_signal;
  AgsNote *note;

  AGS_RECALL_CLASS(ags_feed_audio_signal_parent_class)->run_pre(recall);

  audio_signal = AGS_RECALL_AUDIO_SIGNAL(recall)->source;
  note = audio_signal->note;

  template = NULL;
  
  if(AGS_RECALL_AUDIO_SIGNAL(recall)->source->note != NULL &&
     (AGS_NOTE_FEED & (note->flags)) != 0){
    gdouble notation_delay;
    guint frame_count;

    if(audio_signal->recycling != NULL){
      template = ags_audio_signal_get_template(AGS_RECYCLING(audio_signal->recycling)->audio_signal);
    }
    
    /* get notation delay */
    notation_delay = ags_soundcard_get_absolute_delay(AGS_SOUNDCARD(recall->soundcard));

    /* feed audio signal */
    frame_count = (guint) (((gdouble) audio_signal->samplerate / notation_delay) * (gdouble) (note->x[1] - note->x[0]));
    
    ags_audio_signal_feed(audio_signal,
			  template,
			  frame_count);
  }else{
    ags_recall_done(recall);
  }
}

/**
 * ags_feed_audio_signal_new:
 * @audio_signal: an #AgsAudioSignal
 *
 * Creates an #AgsFeedAudioSignal
 *
 * Returns: a new #AgsFeedAudioSignal
 *
 * Since: 0.7.111
 */
AgsFeedAudioSignal*
ags_feed_audio_signal_new(AgsAudioSignal *audio_signal)
{
  AgsFeedAudioSignal *feed_audio_signal;

  feed_audio_signal = (AgsFeedAudioSignal *) g_object_new(AGS_TYPE_FEED_AUDIO_SIGNAL,
							  NULL);

  return(feed_audio_signal);
}
