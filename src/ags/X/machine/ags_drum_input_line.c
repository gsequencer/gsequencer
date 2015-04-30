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

#include <ags/util/ags_id_generator.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_plugin.h>
#include <ags/object/ags_portlet.h>

#include <ags/file/ags_file.h>
#include <ags/file/ags_file_stock.h>
#include <ags/file/ags_file_id_ref.h>
#include <ags/file/ags_file_lookup.h>

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
gchar* ags_drum_input_line_get_name(AgsPlugin *plugin);
void ags_drum_input_line_set_name(AgsPlugin *plugin, gchar *name);
gchar* ags_drum_input_line_get_xml_type(AgsPlugin *plugin);
void ags_drum_input_line_set_xml_type(AgsPlugin *plugin, gchar *xml_type);
void ags_drum_input_line_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin);
xmlNode* ags_drum_input_line_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin);

void ags_drum_input_line_set_channel(AgsLine *line, AgsChannel *channel);
void ags_drum_input_line_group_changed(AgsLine *line);
void ags_drum_input_line_map_recall(AgsLine *line,
				    guint output_pad_start);

/**
 * SECTION:ags_drum_input_line
 * @short_description: drum sequencer input line
 * @title: AgsDrumInputLine
 * @section_id:
 * @include: ags/X/machine/ags_drum_input_line.h
 *
 * The #AgsDrumInputLine is a composite widget to act as drum sequencer input line.
 */

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

  line->map_recall = ags_drum_input_line_map_recall;
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
  plugin->get_name = ags_drum_input_line_get_name;
  plugin->set_name = ags_drum_input_line_set_name;
  plugin->get_xml_type = ags_drum_input_line_get_xml_type;
  plugin->set_xml_type = ags_drum_input_line_set_xml_type;
  plugin->read = ags_drum_input_line_read;
  plugin->write = ags_drum_input_line_write;
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
}

void
ags_drum_input_line_disconnect(AgsConnectable *connectable)
{
  ags_drum_input_line_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

gchar*
ags_drum_input_line_get_name(AgsPlugin *plugin)
{
  return(AGS_DRUM_INPUT_LINE(plugin)->name);
}

void
ags_drum_input_line_set_name(AgsPlugin *plugin, gchar *name)
{
  AGS_DRUM_INPUT_LINE(plugin)->name = name;
}

gchar*
ags_drum_input_line_get_xml_type(AgsPlugin *plugin)
{
  return(AGS_DRUM_INPUT_LINE(plugin)->xml_type);
}

void
ags_drum_input_line_set_xml_type(AgsPlugin *plugin, gchar *xml_type)
{
  AGS_DRUM_INPUT_LINE(plugin)->xml_type = xml_type;
}

void
ags_drum_input_line_set_channel(AgsLine *line, AgsChannel *channel)
{
  AgsDrumInputLine *drum_input_line;
  AgsChannel *old_channel;
  guint old_flags;
  
  drum_input_line = AGS_DRUM_INPUT_LINE(line);

#ifdef AGS_DEBUG
  g_message("ags_drum_input_line_set_channel - channel: %u\0",
	    channel->line);
#endif

  if(line->channel != NULL){
    old_flags = line->flags;
    old_channel = line->channel;
  }else{
    old_flags = 0;
    old_channel = NULL;
  }

  AGS_LINE_CLASS(ags_drum_input_line_parent_class)->set_channel(line, channel);

  if(channel != NULL){
    if(channel->audio != NULL &&
       AGS_AUDIO(channel->audio)->soundcard != NULL){
      AgsAudioSignal *audio_signal;

      audio_signal = ags_audio_signal_new(AGS_AUDIO(channel->audio)->soundcard,
					  channel->first_recycling,
					  NULL);
      audio_signal->flags |= AGS_AUDIO_SIGNAL_TEMPLATE;
      ags_recycling_add_audio_signal(channel->first_recycling,
				     audio_signal);
    }

    if(channel->pattern == NULL){
      channel->pattern = g_list_alloc();
      channel->pattern->data = (gpointer) ags_pattern_new();
      ags_pattern_set_dim((AgsPattern *) channel->pattern->data, 4, 12, 64);
    }

    /* reset edit button */
    if(old_channel == NULL &&
       line->channel->line == 0){
      AgsDrum *drum;
      GtkToggleButton *selected_edit_button;
      GList *list;

      drum = (AgsDrum *) gtk_widget_get_ancestor(GTK_WIDGET(line),
						 AGS_TYPE_DRUM);

      if(drum != NULL){
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
ags_drum_input_line_map_recall(AgsLine *line,
			       guint output_pad_start)
{
  AgsLineMember *line_member;

  AgsAudio *audio;
  AgsChannel *source;
  AgsChannel *current, *destination;
  AgsPattern *pattern;
  AgsRecallHandler *recall_handler;

  AgsCopyPatternChannel *copy_pattern_channel;
  AgsPlayChannel *play_channel;
  AgsPlayChannelRun *play_channel_run;
  AgsPeakChannelRun *recall_peak_channel_run, *play_peak_channel_run;
  AgsBufferChannel *buffer_channel;
  AgsBufferChannelRun *buffer_channel_run;
  AgsStreamChannelRun *stream_channel_run;

  GList *list;
  guint i;

  if((AGS_LINE_MAPPED_RECALL & (line->flags)) != 0 ||
     (AGS_LINE_PREMAPPED_RECALL & (line->flags)) != 0){
    return;
  }

  audio = AGS_AUDIO(line->channel->audio);

  source = line->channel;

  /* ags-copy-pattern */
  ags_recall_factory_create(audio,
			    NULL, NULL,
			    "ags-copy-pattern\0",
			    source->audio_channel, source->audio_channel + 1, 
			    source->pad, source->pad + 1,
			    (AGS_RECALL_FACTORY_INPUT |
			     AGS_RECALL_FACTORY_REMAP |
			     AGS_RECALL_FACTORY_RECALL),
			    0);

  /* set pattern object on port */
  list = ags_recall_template_find_type(source->recall, AGS_TYPE_COPY_PATTERN_CHANNEL);

  if(list != NULL){
    copy_pattern_channel = AGS_COPY_PATTERN_CHANNEL(list->data);
    list = source->pattern;
    
    pattern = AGS_PATTERN(list->data);
    copy_pattern_channel->pattern->port_value.ags_port_object = (GObject *) pattern;
  
    ags_portlet_set_port(AGS_PORTLET(pattern), copy_pattern_channel->pattern);
  }

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

    g_value_init(&audio_channel_value, G_TYPE_UINT64);
    g_value_set_uint64(&audio_channel_value,
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

  /* play - connect run_post */
  list = ags_recall_template_find_type(source->play,
				       AGS_TYPE_PEAK_CHANNEL_RUN);

  if(list != NULL){
    play_peak_channel_run = AGS_PEAK_CHANNEL_RUN(list->data);

    recall_handler = (AgsRecallHandler *) malloc(sizeof(AgsRecallHandler));

    recall_handler->signal_name = "run-post\0";
    recall_handler->callback = G_CALLBACK(ags_line_peak_run_post_callback);
    recall_handler->data = (gpointer) line;

    ags_recall_add_handler(AGS_RECALL(play_peak_channel_run), recall_handler);
  }

  /* recall - connect run_post */
  list = ags_recall_template_find_type(source->recall,
				       AGS_TYPE_PEAK_CHANNEL_RUN);

  if(list != NULL){
    recall_peak_channel_run = AGS_PEAK_CHANNEL_RUN(list->data);

    recall_handler = (AgsRecallHandler *) malloc(sizeof(AgsRecallHandler));

    recall_handler->signal_name = "run-post\0";
    recall_handler->callback = G_CALLBACK(ags_line_peak_run_post_callback);
    recall_handler->data = (gpointer) line;

    ags_recall_add_handler(AGS_RECALL(recall_peak_channel_run), recall_handler);
  }

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

  AGS_LINE_CLASS(ags_drum_input_line_parent_class)->map_recall(line,
							       output_pad_start);
}

void
ags_drum_input_line_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin)
{
  AgsDrumInputLine *gobject;

  gobject = AGS_DRUM_INPUT_LINE(plugin);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", xmlGetProp(node, AGS_FILE_ID_PROP)),
				   "reference\0", gobject,
				   NULL));
}

xmlNode*
ags_drum_input_line_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin)
{
  AgsDrumInputLine *drum_input_line;
  xmlNode *node;
  gchar *id;

  drum_input_line = AGS_DRUM_INPUT_LINE(plugin);

  id = ags_id_generator_create_uuid();
  
  node = xmlNewNode(NULL,
		    "ags-drum-input-line\0");
  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", drum_input_line,
				   NULL));

  return(node);
}

/**
 * ags_drum_input_line_new:
 * @channel: the assigned channel
 *
 * Creates an #AgsDrumInputLine
 *
 * Returns: a new #AgsDrumInputLine
 *
 * Since: 0.4
 */
AgsDrumInputLine*
ags_drum_input_line_new(AgsChannel *channel)
{
  AgsDrumInputLine *drum_input_line;

  drum_input_line = (AgsDrumInputLine *) g_object_new(AGS_TYPE_DRUM_INPUT_LINE,
						      "channel\0", channel,
						      NULL);

  return(drum_input_line);
}
