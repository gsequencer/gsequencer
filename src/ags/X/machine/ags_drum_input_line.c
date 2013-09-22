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

#include <ags-lib/object/ags_connectable.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_output.h>
#include <ags/audio/ags_pattern.h>
#include <ags/audio/ags_recall.h>
#include <ags/audio/ags_recall_container.h>

#include <ags/audio/recall/ags_delay_audio_run.h>
#include <ags/audio/recall/ags_volume_channel.h>
#include <ags/audio/recall/ags_volume_channel_run.h>
#include <ags/audio/recall/ags_play_channel.h>
#include <ags/audio/recall/ags_play_channel_run.h>
#include <ags/audio/recall/ags_copy_channel.h>
#include <ags/audio/recall/ags_copy_channel_run.h>
#include <ags/audio/recall/ags_stream_channel.h>
#include <ags/audio/recall/ags_stream_channel_run.h>
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
  g_signal_connect_after((GObject *) drum_input_line, "parent_set\0",
			 G_CALLBACK(ags_drum_input_line_parent_set_callback), (gpointer) drum_input_line);

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

  drum_input_line = AGS_DRUM_INPUT_LINE(connectable);


  if((AGS_LINE_CONNECTED & (AGS_LINE(drum_input_line)->flags)) != 0){
    return;
  }
  
  ags_drum_input_line_parent_connectable_interface->connect(connectable);

  /* AgsDrumInputLine */
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

  printf("ags_drum_input_line_set_channel - channel: %u\n\0",
	 channel->line);

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
  AgsChannel *source, *destination, *destination_start;
  AgsRecallContainer *play_copy_pattern_container, *recall_copy_pattern_container, *play_channel_container, *play_volume_channel_container, *recall_volume_channel_container, *play_stream_channel_container, *recall_stream_channel_container, *copy_channel_container;
  AgsVolumeChannel *volume_channel;
  AgsVolumeChannelRun *volume_channel_run;
  AgsPlayChannel *play_channel;
  AgsPlayChannelRun *play_channel_run;
  AgsCopyPatternAudio *play_copy_pattern_audio, *recall_copy_pattern_audio;
  AgsCopyPatternAudioRun *play_copy_pattern_audio_run, *recall_copy_pattern_audio_run;
  AgsCopyPatternChannel *play_copy_pattern_channel, *recall_copy_pattern_channel;
  AgsCopyPatternChannelRun *play_copy_pattern_channel_run, *recall_copy_pattern_channel_run;
  AgsCopyChannel *copy_channel;
  AgsCopyChannelRun *copy_channel_run;
  AgsStreamChannel *play_stream_channel, *recall_stream_channel;
  AgsStreamChannelRun *play_stream_channel_run, *recall_stream_channel_run;
  GList *list;
  guint i;
  GValue recall_container_value = {0,};

  line = AGS_LINE(drum_input_line);

  audio = AGS_AUDIO(line->channel->audio);

  drum = AGS_DRUM(audio->machine);

  source = line->channel;

  destination_start = ags_channel_nth(audio->output,
				      audio->audio_channels * output_pad_start + source->audio_channel);

  /* get some recalls */
  play_copy_pattern_audio = drum->play_copy_pattern_audio;
  play_copy_pattern_audio_run = drum->play_copy_pattern_audio_run;

  recall_copy_pattern_audio = drum->recall_copy_pattern_audio;
  recall_copy_pattern_audio_run = drum->recall_copy_pattern_audio_run;

  /* find AgsRecallContainer pattern copying recalls in audio->play_recall_container */
  g_value_init(&recall_container_value, G_TYPE_OBJECT);
  g_object_get_property(G_OBJECT(play_copy_pattern_audio),
			"recall_container\0",
			&recall_container_value);
  play_copy_pattern_container = (AgsRecallContainer *) g_value_get_object(&recall_container_value);
  g_value_unset(&recall_container_value);

  g_value_init(&recall_container_value, G_TYPE_OBJECT);
  g_object_get_property(G_OBJECT(recall_copy_pattern_audio),
			"recall_container\0",
			&recall_container_value);
  recall_copy_pattern_container = (AgsRecallContainer *) g_value_get_object(&recall_container_value);
  g_value_unset(&recall_container_value);

  if((AGS_DRUM_INPUT_LINE_MAPPED_RECALL & (drum_input_line->flags)) == 0){
    /* recall for channel->play */
    play_stream_channel_container = ags_recall_container_new();
    play_stream_channel_container->flags |= AGS_RECALL_CONTAINER_PLAY;
    ags_channel_add_recall_container(source, (GObject *) play_stream_channel_container);

    /* AgsStreamChannel */
    play_stream_channel = (AgsStreamChannel *) g_object_new(AGS_TYPE_STREAM_CHANNEL,
							    "devout\0", audio->devout,
							    "source\0", source,
							    "recall_container\0", play_stream_channel_container,
							    NULL);
    AGS_RECALL(play_stream_channel)->flags |= (AGS_RECALL_TEMPLATE |
					       AGS_RECALL_PROPAGATE_DONE |
					       AGS_RECALL_INPUT_ORIENTATED);
    ags_channel_add_recall(source, (GObject *) play_stream_channel, TRUE);

    if(GTK_WIDGET_VISIBLE(drum))
      ags_connectable_connect(AGS_CONNECTABLE(play_stream_channel));

    /* AgsStreamChannelRun */
    play_stream_channel_run = (AgsStreamChannelRun *) g_object_new(AGS_TYPE_STREAM_CHANNEL_RUN,
								   "devout\0", audio->devout,
								   "source\0", source,
								   "recall_channel\0", play_stream_channel,
								   "recall_container\0", play_stream_channel_container,
								   NULL);
    AGS_RECALL(play_stream_channel_run)->flags |= (AGS_RECALL_TEMPLATE |
						   AGS_RECALL_PLAYBACK |
						   AGS_RECALL_PROPAGATE_DONE |
						   AGS_RECALL_INPUT_ORIENTATED);
    ags_channel_add_recall(source, (GObject *) play_stream_channel_run, TRUE);

    if(GTK_WIDGET_VISIBLE(drum))
      ags_connectable_connect(AGS_CONNECTABLE(play_stream_channel_run));

    /* recall for channel->recall */
    recall_stream_channel_container = ags_recall_container_new();
    ags_channel_add_recall_container(source, (GObject *) recall_stream_channel_container);

    /* AgsStreamChannel */
    recall_stream_channel = (AgsStreamChannel *) g_object_new(AGS_TYPE_STREAM_CHANNEL,
							      "devout\0", audio->devout,
							      "source\0", source,
							      "recall_container\0", recall_stream_channel_container,
							      NULL);
    AGS_RECALL(recall_stream_channel)->flags |= (AGS_RECALL_TEMPLATE |
						 AGS_RECALL_SEQUENCER |
						 AGS_RECALL_NOTATION |
						 AGS_RECALL_INPUT_ORIENTATED);
    ags_channel_add_recall(source, (GObject *) recall_stream_channel, FALSE);

    if(GTK_WIDGET_VISIBLE(drum))
      ags_connectable_connect(AGS_CONNECTABLE(recall_stream_channel));

    /* AgsStreamChannelRun */
    recall_stream_channel_run = (AgsStreamChannelRun *) g_object_new(AGS_TYPE_STREAM_CHANNEL_RUN,
								     "devout\0", audio->devout,
								     "source\0", source,
								     "recall_channel\0", recall_stream_channel,
								     "recall_container\0", recall_stream_channel_container,
								     NULL);
    AGS_RECALL(recall_stream_channel_run)->flags |= (AGS_RECALL_TEMPLATE |
						     AGS_RECALL_SEQUENCER |
						     AGS_RECALL_NOTATION |
						     AGS_RECALL_INPUT_ORIENTATED);
    ags_channel_add_recall(source, (GObject *) recall_stream_channel_run, FALSE);

    if(GTK_WIDGET_VISIBLE(drum))
      ags_connectable_connect(AGS_CONNECTABLE(recall_stream_channel_run));

    /* AgsPlayChannel */
    play_channel_container = ags_recall_container_new();
    play_channel_container->flags |= AGS_RECALL_CONTAINER_PLAY;
    ags_channel_add_recall_container(source, (GObject *) play_channel_container);

    play_channel = (AgsPlayChannel *) g_object_new(AGS_TYPE_PLAY_CHANNEL,
						   "devout\0", AGS_DEVOUT(audio->devout),
						   "source\0", source,
						   "recall_container\0", play_channel_container,
						   "audio_channel\0", source->audio_channel,
						   NULL);

    AGS_RECALL(play_channel)->flags |= (AGS_RECALL_TEMPLATE |
					AGS_RECALL_PROPAGATE_DONE);
    ags_channel_add_recall(source, (GObject *) play_channel, TRUE);

    if(GTK_WIDGET_VISIBLE(drum))
      ags_connectable_connect(AGS_CONNECTABLE(play_channel));

    /* AgsPlayChannelRun */
    play_channel_run = (AgsPlayChannelRun *) g_object_new(AGS_TYPE_PLAY_CHANNEL_RUN,
							  "devout\0", audio->devout,
							  "source\0", source,
							  "recall_channel\0", play_channel,
							  "recall_container\0", play_channel_container,
							  "stream_channel_run\0", play_stream_channel_run,
							  "audio_channel\0", source->audio_channel,
							  NULL);

    AGS_RECALL(play_channel_run)->flags |= (AGS_RECALL_TEMPLATE |
					    AGS_RECALL_PLAYBACK |
					    AGS_RECALL_PROPAGATE_DONE);
    ags_channel_add_recall(source, (GObject *) play_channel_run, TRUE);

    if(GTK_WIDGET_VISIBLE(drum))
      ags_connectable_connect(AGS_CONNECTABLE(play_channel_run));
    
    ags_recall_add_handler(AGS_RECALL(play_channel_run),
			   ags_recall_handler_alloc("done\0",
						    G_CALLBACK(ags_drum_input_line_play_channel_run_done),
						    G_OBJECT(drum_input_line)));

    //    g_signal_connect((GObject *) play_channel_run, ,
    //		     G_CALLBACK(), drum_input_line);
    
    //    g_signal_connect((GObject *) play_channel_run, "cancel\0",
    //		     G_CALLBACK(ags_drum_input_line_play_channel_run_cancel), );

    /* volume * /
    /* recall for channel->play * /
    recall_volume_channel_container = ags_recall_container_new();
    ags_channel_add_recall_container(source, (GObject *) recall_volume_channel_container);

    /* AgsVolumeChannel * /
    volume_channel = (AgsVolumeChannel *) g_object_new(AGS_TYPE_VOLUME_CHANNEL,
						       "devout\0", audio->devout,
						       "source\0", source,
						       "recall_container\0", recall_volume_channel_container,
						       NULL);
							      
    AGS_RECALL(volume_channel)->flags |= (AGS_RECALL_TEMPLATE |
					  AGS_RECALL_PLAYBACK |
					  AGS_RECALL_PROPAGATE_DONE |
					  AGS_RECALL_OUTPUT_ORIENTATED);
    ags_channel_add_recall(source, (GObject *) volume_channel, TRUE);

    /* AgsVolumeChannelRun * /
    volume_channel_run = (AgsVolumeChannelRun *) g_object_new(AGS_TYPE_VOLUME_CHANNEL_RUN,
							      "devout\0", audio->devout,
							      "recall_channel\0", volume_channel,
							      "source\0", source,
							      "recall_container\0", recall_volume_channel_container,
							      "volume\0", &(GTK_RANGE(drum_input_line->volume)->adjustment->value),
							      NULL);
    
    AGS_RECALL(volume_channel_run)->flags |= (AGS_RECALL_TEMPLATE |
					      AGS_RECALL_PLAYBACK |
					      AGS_RECALL_PROPAGATE_DONE |
					      AGS_RECALL_OUTPUT_ORIENTATED);
    ags_channel_add_recall(source, (GObject *) volume_channel_run, TRUE);

    if(GTK_WIDGET_VISIBLE(drum))
      ags_connectable_connect(AGS_CONNECTABLE(volume_channel_run));

    /* recall for channel->recall * /
    play_volume_channel_container = ags_recall_container_new();
    ags_channel_add_recall_container(source, (GObject *) play_volume_channel_container);

    /* AgsVolumeChannel * /
    volume_channel = (AgsVolumeChannel *) g_object_new(AGS_TYPE_VOLUME_CHANNEL,
						       "devout\0", audio->devout,
						       "source\0", source,
						       "recall_container\0", play_volume_channel_container,
						       NULL);
    
    AGS_RECALL(volume_channel)->flags |= (AGS_RECALL_TEMPLATE |
					  AGS_RECALL_PLAYBACK |
					  AGS_RECALL_PROPAGATE_DONE |
					  AGS_RECALL_OUTPUT_ORIENTATED);
    ags_channel_add_recall(source, (GObject *) volume_channel, FALSE);

    /* AgsVolumeChannelRun * /
    volume_channel_run = (AgsVolumeChannelRun *) g_object_new(AGS_TYPE_VOLUME_CHANNEL_RUN,
							      "devout\0", audio->devout,
							      "recall_channel\0", volume_channel,
							      "source\0", source,
							      "recall_container\0", play_volume_channel_container,
							      "volume\0", &(GTK_RANGE(drum_input_line->volume)->adjustment->value),
							      NULL);
    
    AGS_RECALL(volume_channel_run)->flags |= (AGS_RECALL_TEMPLATE |
					      AGS_RECALL_SEQUENCER |
					      AGS_RECALL_NOTATION |
					      AGS_RECALL_OUTPUT_ORIENTATED);
    ags_channel_add_recall(source, (GObject *) volume_channel_run, FALSE);

    if(GTK_WIDGET_VISIBLE(drum))
      ags_connectable_connect(AGS_CONNECTABLE(volume_channel_run));
    */
  }

  /* create recalls which depend on output */
  destination = destination_start;

  if(destination != NULL)
    printf("ags_drum_input_line_map_recall - destination: %u\n\0",
	   destination->line);


  while(destination != NULL){
    /* recall for channel->recall */
    /* AgsCopyPatternChannel in channel->recall */
    play_copy_pattern_channel = (AgsCopyPatternChannel *) g_object_new(AGS_TYPE_COPY_PATTERN_CHANNEL,
								       "devout\0", AGS_DEVOUT(audio->devout),
								       "source\0", source,
								       "destination\0", destination,
								       "recall_container\0", recall_copy_pattern_container,
								       "pattern\0", source->pattern->data,
								       NULL);
    AGS_RECALL(play_copy_pattern_channel)->flags |= (AGS_RECALL_TEMPLATE |
						     AGS_RECALL_SEQUENCER |
						     AGS_RECALL_NOTATION |
						     AGS_RECALL_INPUT_ORIENTATED);
    ags_channel_add_recall(source, (GObject *) play_copy_pattern_channel, TRUE);
    
    if(GTK_WIDGET_VISIBLE(drum))
      ags_connectable_connect(AGS_CONNECTABLE(play_copy_pattern_channel));

    /* AgsCopyPatternChannelRun */
    play_copy_pattern_channel_run = (AgsCopyPatternChannelRun *) g_object_new(AGS_TYPE_COPY_PATTERN_CHANNEL_RUN,
									      "devout\0", audio->devout,
									      "source\0", source,
									      "destination\0", destination,
									      "recall_channel\0", play_copy_pattern_channel,
									      "recall_audio_run\0", play_copy_pattern_audio_run,
									      "recall_container\0", play_copy_pattern_container,
									      NULL);
  
    AGS_RECALL(play_copy_pattern_channel_run)->flags |= (AGS_RECALL_TEMPLATE |
							 AGS_RECALL_SEQUENCER |
							 AGS_RECALL_NOTATION |
							 AGS_RECALL_INPUT_ORIENTATED);
    ags_channel_add_recall(source, (GObject *) play_copy_pattern_channel_run, TRUE);
    
    if(GTK_WIDGET_VISIBLE(drum))
      ags_connectable_connect(AGS_CONNECTABLE(play_copy_pattern_channel_run));
    
    /* connect callbacks for play */
    g_signal_connect((GObject *) play_copy_pattern_channel_run, "done\0",
		     G_CALLBACK(ags_drum_input_line_copy_pattern_done), drum);
    
    g_signal_connect((GObject *) play_copy_pattern_channel_run, "cancel\0",
		     G_CALLBACK(ags_drum_input_line_copy_pattern_cancel), drum);
  

    /* recall for channel->play */
    /* AgsCopyPatternChannel in channel->recall */
    recall_copy_pattern_channel = (AgsCopyPatternChannel *) g_object_new(AGS_TYPE_COPY_PATTERN_CHANNEL,
									 "devout\0", audio->devout,
									 "source\0", source,
									 "destination\0", destination,
									 "recall_container\0", recall_copy_pattern_container,
									 "pattern\0", source->pattern->data,
									 NULL);
    AGS_RECALL(recall_copy_pattern_channel)->flags |= (AGS_RECALL_TEMPLATE |
						       AGS_RECALL_SEQUENCER |
						       AGS_RECALL_NOTATION |
						       AGS_RECALL_INPUT_ORIENTATED);
    ags_channel_add_recall(source, (GObject *) recall_copy_pattern_channel, FALSE);
      
    if(GTK_WIDGET_VISIBLE(drum))
      ags_connectable_connect(AGS_CONNECTABLE(recall_copy_pattern_channel));

    /* AgsCopyPatternChannelRun */
    recall_copy_pattern_channel_run = (AgsCopyPatternChannelRun *) g_object_new(AGS_TYPE_COPY_PATTERN_CHANNEL_RUN,
										"devout\0", audio->devout,
										"source\0", source,
										"destination\0", destination,
										"recall_channel\0", recall_copy_pattern_channel,
										"recall_audio_run\0", recall_copy_pattern_audio_run,
										"recall_container\0", recall_copy_pattern_container,
										NULL);
    
    AGS_RECALL(recall_copy_pattern_channel_run)->flags |= (AGS_RECALL_TEMPLATE  |
							   AGS_RECALL_SEQUENCER |
							   AGS_RECALL_NOTATION |
							   AGS_RECALL_INPUT_ORIENTATED);
    ags_channel_add_recall(source, (GObject *) recall_copy_pattern_channel_run, FALSE);
    
    if(GTK_WIDGET_VISIBLE(drum))
      ags_connectable_connect(AGS_CONNECTABLE(recall_copy_pattern_channel_run));
 
    /* copy */
    /* recall for channel->recall */
    copy_channel_container = ags_recall_container_new();
    ags_channel_add_recall_container(source, (GObject *) copy_channel_container);

    /* AgsCopyChannel */
    copy_channel = (AgsCopyChannel *) g_object_new(AGS_TYPE_COPY_CHANNEL,
						   "devout\0", audio->devout,
						   "source\0", source,
						   "destination\0", destination,
						   "recall_container\0", copy_channel_container,
						   NULL);
    AGS_RECALL(copy_channel)->flags |= (AGS_RECALL_TEMPLATE |
					AGS_RECALL_SEQUENCER |
					AGS_RECALL_NOTATION |
					AGS_RECALL_OUTPUT_ORIENTATED);
    ags_channel_add_recall(source, (GObject *) copy_channel, FALSE);

    if(GTK_WIDGET_VISIBLE(drum))
      ags_connectable_connect(AGS_CONNECTABLE(copy_channel));

    /* AgsCopyChannelRun */
    copy_channel_run = (AgsCopyChannelRun *) g_object_new(AGS_TYPE_COPY_CHANNEL_RUN,
							  "devout\0", audio->devout,
							  "source\0", source,
							  "destination\0", destination,
							  "recall_channel\0", copy_channel,
							  "recall_container\0", copy_channel_container,
							  NULL);
    AGS_RECALL(copy_channel_run)->flags |= (AGS_RECALL_TEMPLATE |
					    AGS_RECALL_SEQUENCER |
					    AGS_RECALL_NOTATION |
					    AGS_RECALL_OUTPUT_ORIENTATED);
    ags_channel_add_recall(source, (GObject *) copy_channel_run, FALSE);

    if(GTK_WIDGET_VISIBLE(drum))
      ags_connectable_connect(AGS_CONNECTABLE(copy_channel_run));

    /* iterate */
    destination = destination->next_pad;
  }

  drum_input_line->flags |= AGS_DRUM_INPUT_LINE_MAPPED_RECALL;
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
