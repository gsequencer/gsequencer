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
#include <ags/audio/ags_recall_factory.h>
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
  gtk_table_attach(AGS_LINE(drum_input_line)->expander->table,
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
  AgsChannel *source;
  AgsChannel *current, *destination;
  AgsCopyChannel *copy_channel;
  AgsCopyChannelRun *copy_channel_run;

  GList *list;
  guint i;

  line = AGS_LINE(drum_input_line);

  audio = AGS_AUDIO(line->channel->audio);

  drum = AGS_DRUM(audio->machine);

  source = line->channel;

  /* ags-play */
  //  ags_recall_factory_create(audio,
  //			    "ags-play\0",
  //			    0, audio->audio_channels,
  //			    source->pad, source->pad + 1,
  //			    FALSE,
  //			    FALSE);

  /* ags-volume */
  ags_recall_factory_create(audio,
			    NULL, NULL,
			    "ags-volume\0",
			    0, audio->audio_channels,
			    source->pad, source->pad + 1,
			    (AGS_RECALL_FACTORY_RECALL |
			     AGS_RECALL_FACTORY_ADD),
			    0);

  /* ags-copy */
  ags_recall_factory_create(audio,
			    NULL, NULL,
			    "ags-copy\0",
			    0, audio->audio_channels,
			    source->pad, source->pad + 1,
			    (AGS_RECALL_FACTORY_RECALL |
			     AGS_RECALL_FACTORY_ADD),
			    0);

  current = source;
  
  while(current != NULL){
    destination = ags_channel_nth(audio->output,
				  current->audio_channel);

    /* recall */
    list = current->recall;

    while((list = ags_recall_find_type(list, AGS_TYPE_COPY_CHANNEL)) != NULL){
      copy_channel = AGS_COPY_CHANNEL(list->data);

      g_object_set(G_OBJECT(copy_channel),
		   "destination\0", destination,
		   NULL);

      list = list->next;
    }

    list = current->recall;
    
    while((list = ags_recall_find_type(list, AGS_TYPE_COPY_CHANNEL_RUN)) != NULL){
      copy_channel_run = AGS_COPY_CHANNEL_RUN(list->data);

      g_object_set(G_OBJECT(copy_channel_run),
		   "destination\0", destination,
		   NULL);

      list = list->next;
    }

    current = current->next;
  }

  /* ags-stream */
  ags_recall_factory_create(audio,
			    NULL, NULL,
			    "ags-stream\0",
			    0, audio->audio_channels,
			    source->pad, source->pad + 1,
			    (AGS_RECALL_FACTORY_PLAY |
			     AGS_RECALL_FACTORY_RECALL | 
			     AGS_RECALL_FACTORY_ADD),
			    0);

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
