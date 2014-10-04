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

#include <ags/object/ags_plugin.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_output.h>
#include <ags/audio/ags_pattern.h>
#include <ags/audio/ags_recall_factory.h>
#include <ags/audio/ags_recall.h>
#include <ags/audio/ags_recall_container.h>

#include <ags/audio/recall/ags_delay_audio_run.h>
#include <ags/audio/recall/ags_peak_channel.h>
#include <ags/audio/recall/ags_peak_channel_run.h>
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
#include <ags/audio/recall/ags_buffer_channel.h>
#include <ags/audio/recall/ags_buffer_channel_run.h>

#include <ags/widget/ags_expander_set.h>
#include <ags/widget/ags_expander.h>
#include <ags/widget/ags_vindicator.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_line_callbacks.h>
#include <ags/X/ags_line_member.h>

#include <ags/X/machine/ags_drum.h>

void ags_drum_input_line_class_init(AgsDrumInputLineClass *drum_input_line);
void ags_drum_input_line_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_drum_input_line_plugin_interface_init(AgsPluginInterface *plugin);
void ags_drum_input_line_init(AgsDrumInputLine *drum_input_line);
void ags_drum_input_line_destroy(GtkObject *object);
void ags_drum_input_line_connect(AgsConnectable *connectable);
void ags_drum_input_line_disconnect(AgsConnectable *connectable);

void ags_drum_input_line_set_channel(AgsLine *line, AgsChannel *channel);
void ags_drum_input_line_group_changed(AgsLine *line);

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

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_drum_input_line_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_drum_input_line = g_type_register_static(AGS_TYPE_LINE,
						      "AgsDrumInputLine\0", &ags_drum_input_line_info,
						      0);

    g_type_add_interface_static(ags_type_drum_input_line,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_drum_input_line,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
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
  
  line->group_changed = ags_drum_input_line_group_changed;
}

void
ags_drum_input_line_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_drum_input_line_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_drum_input_line_connect;
  connectable->disconnect = ags_drum_input_line_disconnect;
}

void
ags_drum_input_line_plugin_interface_init(AgsPluginInterface *plugin)
{
  /* empty */
}

void
ags_drum_input_line_init(AgsDrumInputLine *drum_input_line)
{
  AgsLineMember *line_member;
  GtkWidget *widget;
  GtkAdjustment *adjustment;

  g_signal_connect_after((GObject *) drum_input_line, "parent_set\0",
			 G_CALLBACK(ags_drum_input_line_parent_set_callback), (gpointer) drum_input_line);

  /* volume indicator */
  line_member = (AgsLineMember *) g_object_new(AGS_TYPE_LINE_MEMBER,
					       "widget-type\0", AGS_TYPE_VINDICATOR,
					       "plugin-name\0", "ags-peak\0",
					       "specifier\0", "./peak[0]\0",
					       "control-port\0", "1/1\0",
					       NULL);
  line_member->flags |= (AGS_LINE_MEMBER_PLAY_CALLBACK_WRITE |
			 AGS_LINE_MEMBER_RECALL_CALLBACK_WRITE);
  ags_expander_add(AGS_LINE(drum_input_line)->expander,
		   GTK_WIDGET(line_member),
		   0, 0,
		   1, 1);
  widget = gtk_bin_get_child(GTK_BIN(line_member));

  adjustment = gtk_adjustment_new(0.0, 0.0, 10.0, 1.0, 1.0, 10.0);
  g_object_set(widget,
	       "adjustment\0", adjustment,
	       NULL);

  gtk_widget_set_size_request(widget,
			      16, 100);
  gtk_widget_queue_draw(widget);

  //TODO:JK: fix me
  //  g_object_set(G_OBJECT(line_member),
  //	       "port-data\0", (gpointer) &(adjustment->value),
  //	       NULL);

  /* volume control */
  line_member = (AgsLineMember *) g_object_new(AGS_TYPE_LINE_MEMBER,
					       "widget-type\0", GTK_TYPE_VSCALE,
					       "plugin-name\0", "ags-volume\0",
					       "specifier\0", "./volume[0]\0",
					       "control-port\0", "1/1\0",
					       NULL);
  ags_expander_add(AGS_LINE(drum_input_line)->expander,
		   GTK_WIDGET(line_member),
		   1, 0,
		   1, 1);

  widget = gtk_bin_get_child(GTK_BIN(line_member));

  gtk_scale_set_digits(GTK_SCALE(widget),
		       3);

  gtk_range_set_range(GTK_RANGE(widget),
		      0.0, 2.00);
  gtk_range_set_increments(GTK_RANGE(widget),
			   0.025, 0.1);
  gtk_range_set_value(GTK_RANGE(widget),
		      1.0);
  gtk_range_set_inverted(GTK_RANGE(widget),
			 TRUE);

  gtk_widget_set_size_request(widget,
			      -1, 100);

  //  g_object_set(G_OBJECT(line_member),
  //	       "port-data\0", (gpointer) &(GTK_RANGE(widget)->adjustment->value),
  //	       NULL);
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
  
  /* AgsDrum */
  drum = AGS_DRUM(gtk_widget_get_ancestor((GtkWidget *) AGS_LINE(drum_input_line)->pad,
					  AGS_TYPE_DRUM));

  /* AgsAudio */
  g_signal_connect_after(G_OBJECT(AGS_MACHINE(drum)->audio), "set_pads\0",
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

#ifdef AGS_DEBUG
  g_message("ags_drum_input_line_set_channel - channel: %u\0",
	    channel->line);
#endif

  if(line->channel != NULL){
    line->flags &= (~AGS_LINE_MAPPED_RECALL);
  }

  if(channel != NULL){
    if(channel->pattern == NULL){
      channel->pattern = g_list_alloc();
      channel->pattern->data = (gpointer) ags_pattern_new();
      ags_pattern_set_dim((AgsPattern *) channel->pattern->data, 4, 12, 64);
    }

    if((AGS_LINE_PREMAPPED_RECALL & (line->flags)) == 0){
      ags_drum_input_line_map_recall(drum_input_line, 0);
      ags_line_find_port(line);
    }else{
      //TODO:JK: make it advanced
      /* reset edit button */
      if(line->channel->line == 0){
	AgsDrum *drum;
	GtkToggleButton *selected_edit_button;
	GList *list;

	drum = (AgsDrum *) gtk_widget_get_ancestor(GTK_WIDGET(line),
						   AGS_TYPE_DRUM);

	list = gtk_container_get_children((GtkContainer *) drum->input_pad);

	drum->selected_pad = AGS_DRUM_INPUT_PAD(list->data);
	drum->selected_edit_button = drum->selected_pad->edit;
	gtk_toggle_button_set_active((GtkToggleButton *) drum->selected_edit_button, TRUE);

	g_list_free(list);
      }
    }
  }
}

void
ags_drum_input_line_group_changed(AgsLine *line)
{
  AgsDrum *drum;

  drum = (AgsDrum *) gtk_widget_get_ancestor(GTK_WIDGET(line), AGS_TYPE_DRUM);
  ags_drum_set_pattern(drum);
}

void
ags_drum_input_line_map_recall(AgsDrumInputLine *drum_input_line,
			       guint output_pad_start)
{
  AgsDrum *drum;
  AgsLine *line;
  AgsLineMember *line_member;

  AgsAudio *audio;
  AgsChannel *source;
  AgsChannel *current, *destination;
  AgsPlayChannel *play_channel;
  AgsPlayChannelRun *play_channel_run;
  AgsBufferChannel *buffer_channel;
  AgsBufferChannelRun *buffer_channel_run;
  AgsStreamChannelRun *stream_channel_run;

  GList *list;
  guint i;

  line = AGS_LINE(drum_input_line);
  line->flags |= AGS_LINE_MAPPED_RECALL;

  audio = AGS_AUDIO(line->channel->audio);

  drum = AGS_DRUM(audio->machine);

  source = line->channel;

  /* ags-play */
  ags_recall_factory_create(audio,
			    NULL, NULL,
  			    "ags-play\0",
  			    source->audio_channel, source->audio_channel + 1, 
  			    source->pad, source->pad + 1,
  			    (AGS_RECALL_FACTORY_INPUT |
			     AGS_RECALL_FACTORY_PLAY |
  			     AGS_RECALL_FACTORY_ADD),
  			    0);

  list = source->play;

  while((list = ags_recall_find_type(list, AGS_TYPE_PLAY_CHANNEL)) != NULL){
    GValue audio_channel_value = {0,};

    play_channel = AGS_PLAY_CHANNEL(list->data);    

    g_value_init(&audio_channel_value, G_TYPE_UINT);
    g_value_set_uint(&audio_channel_value,
		     source->audio_channel);
    ags_port_safe_write(play_channel->audio_channel,
			&audio_channel_value);

    list = list->next;
  }

  /* ags-peak */
  ags_recall_factory_create(audio,
			    NULL, NULL,
			    "ags-peak\0",
			    source->audio_channel, source->audio_channel + 1, 
			    source->pad, source->pad + 1,
			    (AGS_RECALL_FACTORY_INPUT |
			     AGS_RECALL_FACTORY_PLAY |
			     AGS_RECALL_FACTORY_RECALL |
			     AGS_RECALL_FACTORY_ADD),
			    0);

  /* ags-volume */
  ags_recall_factory_create(audio,
			    NULL, NULL,
			    "ags-volume\0",
			    source->audio_channel, source->audio_channel + 1, 
			    source->pad, source->pad + 1,
			    (AGS_RECALL_FACTORY_INPUT |
			     AGS_RECALL_FACTORY_PLAY |
			     AGS_RECALL_FACTORY_RECALL |
			     AGS_RECALL_FACTORY_ADD),
			    0);

  /* ags-buffer */
  ags_recall_factory_create(audio,
			    NULL, NULL,
			    "ags-buffer\0",
			    source->audio_channel, source->audio_channel + 1, 
			    source->pad, source->pad + 1,
			    (AGS_RECALL_FACTORY_INPUT |
			     AGS_RECALL_FACTORY_RECALL |
			     AGS_RECALL_FACTORY_ADD),
			    0);

  current = source;
  destination = ags_channel_nth(audio->output,
				current->audio_channel);

  while(destination != NULL){
    /* recall */
    list = current->recall;

    while((list = ags_recall_find_type(list, AGS_TYPE_BUFFER_CHANNEL)) != NULL){
      buffer_channel = AGS_BUFFER_CHANNEL(list->data);

      g_object_set(G_OBJECT(buffer_channel),
		   "destination\0", destination,
		   NULL);

      list = list->next;
    }

    list = current->recall;
    
    while((list = ags_recall_find_type(list, AGS_TYPE_BUFFER_CHANNEL_RUN)) != NULL){
      buffer_channel_run = AGS_BUFFER_CHANNEL_RUN(list->data);

      g_object_set(G_OBJECT(buffer_channel_run),
		   "destination\0", destination,
		   NULL);

      list = list->next;
    }

    destination = destination->next_pad;
  }

  /* ags-stream */
  ags_recall_factory_create(audio,
			    NULL, NULL,
			    "ags-stream\0",
			    source->audio_channel, source->audio_channel + 1, 
			    source->pad, source->pad + 1,
			    (AGS_RECALL_FACTORY_INPUT |
			     AGS_RECALL_FACTORY_PLAY |
			     AGS_RECALL_FACTORY_RECALL | 
			     AGS_RECALL_FACTORY_ADD),
			    0);

  /* set up dependencies */
  list = ags_recall_find_type(source->play, AGS_TYPE_PLAY_CHANNEL_RUN);
  play_channel_run = AGS_PLAY_CHANNEL_RUN(list->data);

  list = ags_recall_find_type(source->play, AGS_TYPE_STREAM_CHANNEL_RUN);
  stream_channel_run = AGS_STREAM_CHANNEL_RUN(list->data);

  g_object_set(G_OBJECT(play_channel_run),
	       "stream-channel-run\0", stream_channel_run,
	       NULL);
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
