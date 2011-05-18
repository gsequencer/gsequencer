/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <ags/X/machine/ags_drum_output_line.h>
#include <ags/X/machine/ags_drum_output_line_callbacks.h>

#include <ags/object/ags_connectable.h>

#include <ags/audio/recall/ags_delay_audio.h>
#include <ags/audio/recall/ags_delay_audio_run.h>
#include <ags/audio/recall/ags_copy_pattern_audio_run.h>
#include <ags/audio/recall/ags_loop_channel.h>
#include <ags/audio/recall/ags_volume_channel.h>
#include <ags/audio/recall/ags_stream_channel.h>

#include <ags/X/ags_window.h>

#include <ags/X/machine/ags_drum.h>

void ags_drum_output_line_class_init(AgsDrumOutputLineClass *drum_output_line);
void ags_drum_output_line_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_drum_output_line_init(AgsDrumOutputLine *drum_output_line);
void ags_drum_output_line_destroy(GtkObject *object);
void ags_drum_output_line_connect(AgsConnectable *connectable);
void ags_drum_output_line_disconnect(AgsConnectable *connectable);

void ags_drum_output_line_set_channel(AgsLine *line, AgsChannel *channel);

static gpointer ags_drum_output_line_parent_class = NULL;
static AgsConnectableInterface *ags_drum_output_line_parent_connectable_interface;

GType
ags_drum_output_line_get_type()
{
  static GType ags_type_drum_output_line = 0;

  if(!ags_type_drum_output_line){
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
						       "AgsDrumOutputLine\0", &ags_drum_output_line_info,
						       0);

    g_type_add_interface_static(ags_type_drum_output_line,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_drum_output_line);
}

void
ags_drum_output_line_class_init(AgsDrumOutputLineClass *drum_output_line)
{
  AgsLineClass *line;

  ags_drum_output_line_parent_class = g_type_class_peek_parent(drum_output_line);

  /* AgsLineClass */
  line = AGS_LINE_CLASS(drum_output_line);

  line->set_channel = ags_drum_output_line_set_channel;
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
  g_signal_connect_after((GObject *) drum_output_line, "parent_set\0",
			 G_CALLBACK(ags_drum_output_line_parent_set_callback), NULL);
  
  drum_output_line->flags = 0;
}

void
ags_drum_output_line_destroy(GtkObject *object)
{
}

void
ags_drum_output_line_connect(AgsConnectable *connectable)
{
  ags_drum_output_line_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_drum_output_line_disconnect(AgsConnectable *connectable)
{
  ags_drum_output_line_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_drum_output_line_set_channel(AgsLine *line, AgsChannel *channel)
{
  AgsDrumOutputLine *drum_output_line;

  AGS_LINE_CLASS(ags_drum_output_line_parent_class)->set_channel(line, channel);

  drum_output_line = AGS_DRUM_OUTPUT_LINE(line);

  if(line->channel != NULL){
    drum_output_line->flags &= (~AGS_DRUM_OUTPUT_LINE_MAPPED_RECALL);
  }

  if(channel != NULL){
    AgsDrum *drum;
    AgsAudioSignal *audio_signal;
    AgsDelayAudio *delay_audio;
    GList *list;
    guint stop;

    drum = (AgsDrum *) gtk_widget_get_ancestor(GTK_WIDGET(line->pad), AGS_TYPE_DRUM);

    stop = 1;

    if(drum != NULL){
      list = ags_recall_find_type(AGS_AUDIO(drum->machine.audio)->play,
				  AGS_TYPE_DELAY_AUDIO);
      
      if(list != NULL){
	delay_audio = (AgsDelayAudio *) list->data;
	stop = ((guint)drum->length_spin->adjustment->value) * (delay_audio->delay + 1);
      }
    }
    
    audio_signal = ags_audio_signal_get_template(channel->first_recycling->audio_signal);
    ags_audio_signal_stream_resize(audio_signal, stop);

    ags_drum_output_line_map_recall(drum_output_line);
  }
}

void
ags_drum_output_line_map_recall(AgsDrumOutputLine *drum_output_line)
{
  if((AGS_DRUM_OUTPUT_LINE_MAPPED_RECALL & (drum_output_line->flags)) == 0){
    AgsDrum *drum;
    AgsAudio *audio;
    AgsChannel *output;
    AgsCountBeatsAudioRun *play_count_beats_audio_run, *recall_count_beats_audio_run;
    AgsLoopChannel *loop_channel;
    AgsStreamChannel *stream_channel;

    drum_output_line->flags |= AGS_DRUM_OUTPUT_LINE_MAPPED_RECALL;

    output = AGS_LINE(drum_output_line)->channel;
    audio = AGS_AUDIO(output->audio);

    drum = AGS_DRUM(audio->machine);

    /* get some recalls */
    play_count_beats_audio_run = drum->play_count_beats_audio_run;
    recall_count_beats_audio_run = drum->recall_count_beats_audio_run;

    /* AgsLoopChannel in channel->play */
    loop_channel = ags_loop_channel_new(output,
					play_count_beats_audio_run,
					TRUE);
    AGS_RECALL(loop_channel)->flags |= AGS_RECALL_TEMPLATE;
    ags_channel_add_recall(output, (GObject *) loop_channel, TRUE);

    if(GTK_WIDGET_VISIBLE(drum))
      ags_connectable_connect(AGS_CONNECTABLE(loop_channel));

    /* AgsLoopChannel in channel->recall */
    loop_channel = ags_loop_channel_new(output,
					recall_count_beats_audio_run,
					TRUE);
    AGS_RECALL(loop_channel)->flags |= AGS_RECALL_TEMPLATE;
    ags_channel_add_recall(output, (GObject *) loop_channel, FALSE);

    if(GTK_WIDGET_VISIBLE(drum))
      ags_connectable_connect(AGS_CONNECTABLE(loop_channel));

    /* AgsStreamChannel in channel->play */
    stream_channel = ags_stream_channel_new(output);
    AGS_RECALL(stream_channel)->flags |= AGS_RECALL_TEMPLATE;
    ags_channel_add_recall(output, (GObject *) stream_channel, TRUE);

    if(GTK_WIDGET_VISIBLE(drum))
      ags_connectable_connect(AGS_CONNECTABLE(stream_channel));

    /* AgsStreamChannel in channel->recall */
    stream_channel = ags_stream_channel_new(output);
    AGS_RECALL(stream_channel)->flags |= AGS_RECALL_TEMPLATE;
    ags_channel_add_recall(output, (GObject *) stream_channel, FALSE);

    if(GTK_WIDGET_VISIBLE(drum))
      ags_connectable_connect(AGS_CONNECTABLE(stream_channel));
  }
}

AgsDrumOutputLine*
ags_drum_output_line_new(AgsChannel *channel)
{
  AgsDrumOutputLine *drum_output_line;

  drum_output_line = (AgsDrumOutputLine *) g_object_new(AGS_TYPE_DRUM_OUTPUT_LINE,
							"channel\0", channel,
							NULL);

  return(drum_output_line);
}
