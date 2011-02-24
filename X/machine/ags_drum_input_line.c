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

#include <ags/X/machine/ags_drum_input_line.h>
#include <ags/X/machine/ags_drum_input_line_callbacks.h>

#include <ags/object/ags_connectable.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_output.h>
#include <ags/audio/ags_pattern.h>
#include <ags/audio/ags_recall.h>
#include <ags/audio/ags_recall_container.h>

#include <ags/audio/recall/ags_delay_audio_run.h>
#include <ags/audio/recall/ags_volume_channel.h>
#include <ags/audio/recall/ags_play_channel.h>
#include <ags/audio/recall/ags_copy_channel.h>
#include <ags/audio/recall/ags_stream_channel.h>
#include <ags/audio/recall/ags_copy_pattern_audio.h>
#include <ags/audio/recall/ags_copy_pattern_audio_run.h>
#include <ags/audio/recall/ags_copy_pattern_channel.h>
#include <ags/audio/recall/ags_copy_pattern_channel_run.h>

#include <ags/X/ags_window.h>

#include <ags/X/machine/ags_drum.h>

void ags_drum_input_line_class_init(AgsDrumInputLineClass *drum_input_line);
void ags_drum_input_line_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_drum_input_line_init(AgsDrumInputLine *drum_input_line);
void ags_drum_input_line_destroy(GtkObject *object);
void ags_drum_input_line_connect(AgsConnectable *connectable);
void ags_drum_input_line_disconnect(AgsConnectable *connectable);

void ags_drum_input_line_set_channel(AgsLine *line, AgsChannel *channel);

static gpointer ags_drum_input_line_parent_class = NULL;
static AgsConnectableInterface *ags_drum_input_line_parent_connectable_interface;

GType
ags_drum_input_line_get_type()
{
  static GType ags_type_drum_input_line = 0;

  if(!ags_type_drum_input_line){
    static const GTypeInfo ags_drum_input_line_info = {
      sizeof(AgsDrumInputLineClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_drum_input_line_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsDrumInputLine),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_drum_input_line_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_drum_input_line_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_drum_input_line = g_type_register_static(AGS_TYPE_LINE,
						      "AgsDrumInputLine\0", &ags_drum_input_line_info,
						      0);

    g_type_add_interface_static(ags_type_drum_input_line,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_drum_input_line);
}

void
ags_drum_input_line_class_init(AgsDrumInputLineClass *drum_input_line)
{
  AgsLineClass *line;

  ags_drum_input_line_parent_class = g_type_class_peek_parent(drum_input_line);

  /* AgsLineClass */
  line = AGS_LINE_CLASS(drum_input_line);

  line->set_channel = ags_drum_input_line_set_channel;
}

void
ags_drum_input_line_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_drum_input_line_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_drum_input_line_connect;
  connectable->disconnect = ags_drum_input_line_disconnect;
}

void
ags_drum_input_line_init(AgsDrumInputLine *drum_input_line)
{
  drum_input_line->flags = 0;

  drum_input_line->volume = (GtkVScale *) gtk_vscale_new_with_range(0.0, 2.00, 0.025);
  gtk_range_set_value((GtkRange *) drum_input_line->volume, 1.0);
  gtk_range_set_inverted((GtkRange *) drum_input_line->volume, TRUE);
  gtk_scale_set_digits((GtkScale *) drum_input_line->volume, 3);
  gtk_widget_set_size_request((GtkWidget *) drum_input_line->volume, -1, 100);
  gtk_table_attach(AGS_LINE(drum_input_line)->table,
		   (GtkWidget *) drum_input_line->volume,
		   0, 1,
		   1, 2,
		   GTK_EXPAND, GTK_EXPAND,
		   0, 0);
}

void
ags_drum_input_line_destroy(GtkObject *object)
{
}

void
ags_drum_input_line_connect(AgsConnectable *connectable)
{
  AgsDrum *drum;
  AgsDrumInputLine *drum_input_line;

  ags_drum_input_line_parent_connectable_interface->connect(connectable);

  /* AgsDrumInputLine */
  drum_input_line = AGS_DRUM_INPUT_LINE(connectable);
  drum = AGS_DRUM(gtk_widget_get_ancestor((GtkWidget *) drum_input_line->line.pad, AGS_TYPE_DRUM));

  /* AgsAudio */
  g_signal_connect_after(G_OBJECT(drum->machine.audio), "set_pads\0",
			 G_CALLBACK(ags_drum_input_line_audio_set_pads_callback), drum_input_line);
}

void
ags_drum_input_line_disconnect(AgsConnectable *connectable)
{
  ags_drum_input_line_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_drum_input_line_set_channel(AgsLine *line, AgsChannel *channel)
{
  AgsDrumInputLine *drum_input_line;

  AGS_LINE_CLASS(ags_drum_input_line_parent_class)->set_channel(line, channel);

  drum_input_line = AGS_DRUM_INPUT_LINE(line);

  if(line->channel != NULL){
    drum_input_line->flags &= (~AGS_DRUM_INPUT_LINE_MAPPED_RECALL);
  }

  if(channel != NULL){
    channel->pattern = g_list_alloc();
    channel->pattern->data = (gpointer) ags_pattern_new();
    ags_pattern_set_dim((AgsPattern *) channel->pattern->data, 4, 12, 64);

    ags_drum_input_line_map_recall(drum_input_line, 0);
  }
}

void
ags_drum_input_line_map_recall(AgsDrumInputLine *drum_input_line,
			       guint output_pad_start)
{
  AgsDrum *drum;
  AgsLine *line;
  AgsAudio *audio;
  AgsChannel *source, *destination;
  AgsRecallContainer *play_container, *recall_container;
  AgsVolumeChannel *volume_channel;
  AgsPlayChannel *play_channel;
  AgsCopyPatternAudio *play_copy_pattern_audio, *recall_copy_pattern_audio;
  AgsCopyPatternChannel *copy_pattern_channel;
  AgsCopyPatternChannelRun *copy_pattern_channel_run;
  AgsCopyChannel *copy_channel;
  AgsStreamChannel *stream_channel;
  GList *list;
  guint i;
  GValue recall_container_value = {0,};

  line = AGS_LINE(drum_input_line);

  audio = AGS_AUDIO(line->channel->audio);

  drum = AGS_DRUM(audio->machine);

  source = line->channel;

  /* get some recalls */
  list = ags_recall_template_find_type(audio->play, AGS_TYPE_COPY_PATTERN_AUDIO);
  play_copy_pattern_audio = AGS_COPY_PATTERN_AUDIO(list->data);

  list = ags_recall_template_find_type(audio->recall, AGS_TYPE_COPY_PATTERN_AUDIO);
  recall_copy_pattern_audio = AGS_COPY_PATTERN_AUDIO(list->data);

  /* find AgsRecallContainer pattern copying recalls in audio->play_recall_container */
  g_value_init(&recall_container_value, G_TYPE_OBJECT);
  g_object_get_property(G_OBJECT(play_copy_pattern_audio),
			"recall_container\0",
			&recall_container_value);
  play_container = (AgsRecallContainer *) g_value_get_object(&recall_container_value);
  g_value_unset(&recall_container_value);

  g_value_init(&recall_container_value, G_TYPE_OBJECT);
  g_object_get_property(G_OBJECT(recall_copy_pattern_audio),
			"recall_container\0",
			&recall_container_value);
  recall_container = (AgsRecallContainer *) g_value_get_object(&recall_container_value);
  g_value_unset(&recall_container_value);

  if((AGS_DRUM_INPUT_LINE_MAPPED_RECALL & (drum_input_line->flags)) == 0){
    drum_input_line->flags |= AGS_DRUM_INPUT_LINE_MAPPED_RECALL;

    /* AgsVolumeChannel */
    volume_channel = ags_volume_channel_new(source,
					    &(GTK_RANGE(drum_input_line->volume)->adjustment->value));
    
    AGS_RECALL(volume_channel)->flags |= AGS_RECALL_TEMPLATE;
    ags_channel_add_recall(source, (GObject *) volume_channel, TRUE);

    if(GTK_WIDGET_VISIBLE(drum))
      ags_connectable_connect(AGS_CONNECTABLE(volume_channel));

    /* AgsPlayChannel */
    play_channel = ags_play_channel_new(source,
					AGS_DEVOUT(audio->devout),
					source->audio_channel);
    
    AGS_RECALL(play_channel)->flags |= AGS_RECALL_TEMPLATE;
    ags_channel_add_recall(source, (GObject *) play_channel, TRUE);

    if(GTK_WIDGET_VISIBLE(drum))
      ags_connectable_connect(AGS_CONNECTABLE(play_channel));
    
    g_signal_connect((GObject *) play_channel, "done\0",
		     G_CALLBACK(ags_drum_input_line_play_channel_done), drum_input_line);
    
    g_signal_connect((GObject *) play_channel, "cancel\0",
		     G_CALLBACK(ags_drum_input_line_play_channel_cancel), drum_input_line);

    /* AgsStreamChannel */
    stream_channel = ags_stream_channel_new(source);
    AGS_RECALL(stream_channel)->flags |= AGS_RECALL_TEMPLATE;
    ags_channel_add_recall(source, (GObject *) stream_channel, TRUE);

    if(GTK_WIDGET_VISIBLE(drum))
      ags_connectable_connect(AGS_CONNECTABLE(stream_channel));
  }

  /* create recalls which depend on output */
  destination = ags_channel_nth(audio->output, audio->audio_channels * output_pad_start + source->audio_channel);


  while(destination != NULL){
    /* recall for channel->play */
    /* AgsCopyPatternChannelRun */
    copy_pattern_channel_run = (AgsCopyPatternChannelRun *) g_object_new(AGS_TYPE_COPY_PATTERN_CHANNEL_RUN,
									 "recall_container\0", play_container,
									 NULL);

    AGS_RECALL(copy_pattern_channel_run)->flags |= AGS_RECALL_TEMPLATE;
    ags_channel_add_recall(source, (GObject *) copy_pattern_channel_run, TRUE);
   
    if(GTK_WIDGET_VISIBLE(drum))
      ags_connectable_connect(AGS_CONNECTABLE(copy_pattern_channel_run));
 
    /* connect callbacks for play */
    g_signal_connect((GObject *) copy_pattern_channel_run, "done\0",
		     G_CALLBACK(ags_drum_input_line_copy_pattern_done), drum);
    
    g_signal_connect((GObject *) copy_pattern_channel_run, "cancel\0",
		     G_CALLBACK(ags_drum_input_line_copy_pattern_cancel), drum);
    
    g_signal_connect((GObject *) copy_pattern_channel_run, "loop\0",
		     G_CALLBACK(ags_drum_input_line_copy_pattern_loop), drum);
      
    destination = destination->next;
    
    /* recall for channel->recall */
    /* AgsCopyChannel */
    copy_channel = (AgsCopyChannel *) g_object_new(AGS_TYPE_COPY_CHANNEL,
						   "source\0", source,
						   "destination\0", destination,
						   "devout\0", audio->devout,
						   NULL);
    AGS_RECALL(copy_channel)->flags |= AGS_RECALL_TEMPLATE;
    ags_channel_add_recall(source, (GObject *) copy_pattern_channel, FALSE);
    
    if(GTK_WIDGET_VISIBLE(drum))
      ags_connectable_connect(AGS_CONNECTABLE(copy_channel));
    
    /* AgsCopyPatternChannelRun */
    copy_pattern_channel_run = (AgsCopyPatternChannelRun *) g_object_new(AGS_TYPE_COPY_PATTERN_CHANNEL_RUN,
									 "recall_container\0", recall_container,
									 NULL);
    
    AGS_RECALL(copy_pattern_channel_run)->flags |= AGS_RECALL_TEMPLATE;
    ags_channel_add_recall(source, (GObject *) copy_pattern_channel_run, FALSE);
    
    if(GTK_WIDGET_VISIBLE(drum))
      ags_connectable_connect(AGS_CONNECTABLE(copy_pattern_channel_run));
  }
}

AgsDrumInputLine*
ags_drum_input_line_new(AgsChannel *channel)
{
  AgsDrumInputLine *drum_input_line;

  drum_input_line = (AgsDrumInputLine *) g_object_new(AGS_TYPE_DRUM_INPUT_LINE,
						      "channel\0", channel,
						      NULL);

  return(drum_input_line);
}
