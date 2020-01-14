/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#include <ags/X/machine/ags_drum_output_line.h>
#include <ags/X/machine/ags_drum_output_line_callbacks.h>

#include <ags/X/ags_window.h>

#include <ags/X/machine/ags_drum.h>

#include <math.h>

void ags_drum_output_line_class_init(AgsDrumOutputLineClass *drum_output_line);
void ags_drum_output_line_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_drum_output_line_init(AgsDrumOutputLine *drum_output_line);

void ags_drum_output_line_connect(AgsConnectable *connectable);
void ags_drum_output_line_disconnect(AgsConnectable *connectable);

void ags_drum_output_line_set_channel(AgsLine *line, AgsChannel *channel);
void ags_drum_output_line_map_recall(AgsLine *line,
				     guint output_pad_start);

/**
 * SECTION:ags_drum_output_line
 * @short_description: drum sequencer output line
 * @title: AgsDrumOutputLine
 * @section_id:
 * @include: ags/X/machine/ags_drum_output_line.h
 *
 * The #AgsDrumOutputLine is a composite widget to act as drum sequencer output line.
 */

static gpointer ags_drum_output_line_parent_class = NULL;
static AgsConnectableInterface *ags_drum_output_line_parent_connectable_interface;

GType
ags_drum_output_line_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_drum_output_line = 0;

    static const GTypeInfo ags_drum_output_line_info = {
      sizeof(AgsDrumOutputLineClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_drum_output_line_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsDrumOutputLine),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_drum_output_line_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_drum_output_line_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_drum_output_line = g_type_register_static(AGS_TYPE_LINE,
						       "AgsDrumOutputLine", &ags_drum_output_line_info,
						       0);

    g_type_add_interface_static(ags_type_drum_output_line,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_drum_output_line);
  }

  return g_define_type_id__volatile;
}

void
ags_drum_output_line_class_init(AgsDrumOutputLineClass *drum_output_line)
{
  AgsLineClass *line;

  ags_drum_output_line_parent_class = g_type_class_peek_parent(drum_output_line);

  /* AgsLineClass */
  line = AGS_LINE_CLASS(drum_output_line);

  line->set_channel = ags_drum_output_line_set_channel;
  line->map_recall = ags_drum_output_line_map_recall;
}

void
ags_drum_output_line_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_drum_output_line_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_drum_output_line_connect;
  connectable->disconnect = ags_drum_output_line_disconnect;
}

void
ags_drum_output_line_init(AgsDrumOutputLine *drum_output_line)
{
  drum_output_line->xml_type = "ags-drum-output-line";
}

void
ags_drum_output_line_connect(AgsConnectable *connectable)
{
  AgsDrum *drum;
  AgsDrumOutputLine *drum_output_line;

  drum_output_line = AGS_DRUM_OUTPUT_LINE(connectable);

  if((AGS_LINE_CONNECTED & (AGS_LINE(drum_output_line)->flags)) != 0){
    return;
  }
  
  ags_drum_output_line_parent_connectable_interface->connect(connectable);

  drum = (AgsDrum *) gtk_widget_get_ancestor((GtkWidget *) drum_output_line,
					     AGS_TYPE_DRUM);
  
  g_signal_connect_after((GObject *) drum, "resize-pads",
			 G_CALLBACK(ags_drum_output_line_resize_pads_callback), drum_output_line);
}

void
ags_drum_output_line_disconnect(AgsConnectable *connectable)
{
  AgsDrum *drum;
  AgsDrumOutputLine *drum_output_line;

  drum_output_line = AGS_DRUM_OUTPUT_LINE(connectable);

  if((AGS_LINE_CONNECTED & (AGS_LINE(drum_output_line)->flags)) == 0){
    return;
  }

  ags_drum_output_line_parent_connectable_interface->disconnect(connectable);

  drum = (AgsDrum *) gtk_widget_get_ancestor((GtkWidget *) drum_output_line,
					     AGS_TYPE_DRUM);

  g_signal_handlers_disconnect_by_data(drum,
				       drum_output_line);
}

void
ags_drum_output_line_set_channel(AgsLine *line, AgsChannel *channel)
{  
  AgsRecycling *first_recycling;
  AgsAudioSignal *audio_signal;

  GObject *output_soundcard;

  /* call parent */
  AGS_LINE_CLASS(ags_drum_output_line_parent_class)->set_channel(line, channel);

  if(channel != NULL){
    ags_channel_set_ability_flags(channel, (AGS_SOUND_ABILITY_SEQUENCER |
					    AGS_SOUND_ABILITY_NOTATION));

    g_object_get(channel,
		 "output-soundcard", &output_soundcard,
		 "first-recycling", &first_recycling,
		 NULL);

    /* instantiate template audio signal */
    audio_signal = ags_audio_signal_new((GObject *) output_soundcard,
					(GObject *) first_recycling,
					NULL);
    audio_signal->flags |= AGS_AUDIO_SIGNAL_TEMPLATE;
    ags_recycling_add_audio_signal(first_recycling,
				   audio_signal);

    if(output_soundcard != NULL){
      g_object_unref(output_soundcard);
    }

    g_object_unref(first_recycling);
  }
}

void
ags_drum_output_line_map_recall(AgsLine *line,
				guint output_pad_start)
{
  AgsAudio *audio;
  AgsChannel *output;

  guint input_pads;
  guint audio_channels;

  if((AGS_LINE_MAPPED_RECALL & (line->flags)) != 0 ||
     (AGS_LINE_PREMAPPED_RECALL & (line->flags)) != 0){
    return;
  }

  output = line->channel;

  /* get some fields */
  g_object_get(output,
	       "audio", &audio,
	       NULL);

  /* get some fields */
  g_object_get(audio,
	       "input-pads", &input_pads,
	       "audio-channels", &audio_channels,
	       NULL);
  
  /* remap for input */  
  if(ags_recall_global_get_rt_safe() ||
     ags_recall_global_get_performance_mode()){
    /* ags-copy */
    ags_recall_factory_create(audio,
			      NULL, NULL,
			      "ags-copy",
			      0, audio_channels, 
			      0, input_pads,
			      (AGS_RECALL_FACTORY_INPUT |
			       AGS_RECALL_FACTORY_RECALL |
			       AGS_RECALL_FACTORY_ADD),
			      0);
  }else{
    /* ags-buffer */
    ags_recall_factory_create(audio,
			      NULL, NULL,
			      "ags-buffer",
			      0, audio_channels, 
			      0, input_pads,
			      (AGS_RECALL_FACTORY_INPUT |
			       AGS_RECALL_FACTORY_RECALL |
			       AGS_RECALL_FACTORY_ADD),
			      0);
  }
  
  if(!(ags_recall_global_get_rt_safe() ||
       ags_recall_global_get_performance_mode())){
    guint pad, audio_channel;

    /* get some fields */
    g_object_get(output,
		 "pad", &pad,
		 "audio-channel", &audio_channel,
		 NULL);

    /* ags-stream */
    ags_recall_factory_create(audio,
			      NULL, NULL,
			      "ags-stream",
			      audio_channel, audio_channel + 1,
			      pad, pad + 1,
			      (AGS_RECALL_FACTORY_OUTPUT |
			       AGS_RECALL_FACTORY_PLAY |
			       AGS_RECALL_FACTORY_RECALL | 
			       AGS_RECALL_FACTORY_ADD),
			      0);
  }

  /* call parent */
  AGS_LINE_CLASS(ags_drum_output_line_parent_class)->map_recall(line,
								output_pad_start);

  g_object_unref(audio);
}

/**
 * ags_drum_output_line_new:
 * @channel: the assigned channel
 *
 * Create a new instance of #AgsDrumOutputLine
 *
 * Returns: the new #AgsDrumOutputLine
 *
 * Since: 3.0.0
 */
AgsDrumOutputLine*
ags_drum_output_line_new(AgsChannel *channel)
{
  AgsDrumOutputLine *drum_output_line;

  drum_output_line = (AgsDrumOutputLine *) g_object_new(AGS_TYPE_DRUM_OUTPUT_LINE,
							"channel", channel,
							NULL);

  return(drum_output_line);
}
