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

#include <ags/X/machine/ags_panel_input_line.h>
#include <ags/X/machine/ags_panel_input_line_callbacks.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_soundcard.h>

#include <ags/object/ags_plugin.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_audio_connection.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_recall_factory.h>
#include <ags/audio/ags_recall.h>
#include <ags/audio/ags_recall_container.h>

#include <ags/audio/recall/ags_play_channel.h>
#include <ags/audio/recall/ags_play_channel_run_master.h>
#include <ags/audio/recall/ags_mute_channel.h>
#include <ags/audio/recall/ags_mute_channel_run.h>

#include <ags/widget/ags_expander_set.h>
#include <ags/widget/ags_expander.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_line_callbacks.h>
#include <ags/X/ags_line_member.h>

#include <ags/X/machine/ags_panel.h>

void ags_panel_input_line_class_init(AgsPanelInputLineClass *panel_input_line);
void ags_panel_input_line_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_panel_input_line_plugin_interface_init(AgsPluginInterface *plugin);
void ags_panel_input_line_init(AgsPanelInputLine *panel_input_line);
void ags_panel_input_line_connect(AgsConnectable *connectable);
void ags_panel_input_line_disconnect(AgsConnectable *connectable);
void ags_panel_input_line_finalize(GObject *gobject);
gchar* ags_panel_input_line_get_name(AgsPlugin *plugin);
void ags_panel_input_line_set_name(AgsPlugin *plugin, gchar *name);
gchar* ags_panel_input_line_get_xml_type(AgsPlugin *plugin);
void ags_panel_input_line_set_xml_type(AgsPlugin *plugin, gchar *xml_type);

void ags_panel_input_line_show(GtkWidget *line);
void ags_panel_input_line_show_all(GtkWidget *line);

void ags_panel_input_line_set_channel(AgsLine *line, AgsChannel *channel);
void ags_panel_input_line_group_changed(AgsLine *line);
void ags_panel_input_line_map_recall(AgsLine *line,
				     guint output_pad_start);

/**
 * SECTION:ags_panel_input_line
 * @short_description: panel input line
 * @title: AgsPanelInputLine
 * @section_id:
 * @include: ags/X/machine/ags_panel_input_line.h
 *
 * The #AgsPanelInputLine is a composite widget to act as panel input line.
 */

static gpointer ags_panel_input_line_parent_class = NULL;
static AgsConnectableInterface *ags_panel_input_line_parent_connectable_interface;

GType
ags_panel_input_line_get_type()
{
  static GType ags_type_panel_input_line = 0;

  if(!ags_type_panel_input_line){
    static const GTypeInfo ags_panel_input_line_info = {
      sizeof(AgsPanelInputLineClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_panel_input_line_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsPanelInputLine),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_panel_input_line_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_panel_input_line_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_panel_input_line_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_panel_input_line = g_type_register_static(AGS_TYPE_LINE,
						       "AgsPanelInputLine", &ags_panel_input_line_info,
						       0);

    g_type_add_interface_static(ags_type_panel_input_line,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_panel_input_line,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return(ags_type_panel_input_line);
}

void
ags_panel_input_line_class_init(AgsPanelInputLineClass *panel_input_line)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;
  AgsLineClass *line;

  ags_panel_input_line_parent_class = g_type_class_peek_parent(panel_input_line);

  /* GObjectClass */
  gobject = (GObjectClass *) panel_input_line;

  gobject->finalize = ags_panel_input_line_finalize;

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) panel_input_line;

  widget->show = ags_panel_input_line_show;
  widget->show_all = ags_panel_input_line_show_all;

  /* AgsLineClass */
  line = AGS_LINE_CLASS(panel_input_line);

  line->set_channel = ags_panel_input_line_set_channel;
  line->map_recall = ags_panel_input_line_map_recall;
}

void
ags_panel_input_line_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_panel_input_line_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_panel_input_line_connect;
  connectable->disconnect = ags_panel_input_line_disconnect;
}

void
ags_panel_input_line_plugin_interface_init(AgsPluginInterface *plugin)
{
  plugin->get_name = ags_panel_input_line_get_name;
  plugin->set_name = ags_panel_input_line_set_name;
  plugin->get_xml_type = ags_panel_input_line_get_xml_type;
  plugin->set_xml_type = ags_panel_input_line_set_xml_type;
}

void
ags_panel_input_line_init(AgsPanelInputLine *panel_input_line)
{
  AgsLineMember *line_member;

  panel_input_line->soundcard_connection = (GtkLabel *) gtk_label_new("(null)");
  ags_expander_add(AGS_LINE(panel_input_line)->expander,
		   GTK_WIDGET(panel_input_line->soundcard_connection),
		   0, 0,
		   1, 1);

  line_member = (AgsLineMember *) g_object_new(AGS_TYPE_LINE_MEMBER,
					       "widget-type", GTK_TYPE_CHECK_BUTTON,
					       "widget-label", "mute",
					       "plugin-name", "ags-play",
					       "specifier", "./muted[0]",
					       "control-port", "2/2",
					       NULL);
  ags_expander_add(AGS_LINE(panel_input_line)->expander,
		   GTK_WIDGET(line_member),
		   1, 0,
		   1, 1);
}

void
ags_panel_input_line_connect(AgsConnectable *connectable)
{
  AgsPanelInputLine *panel_input_line;

  panel_input_line = AGS_PANEL_INPUT_LINE(connectable);

  if((AGS_LINE_CONNECTED & (AGS_LINE(panel_input_line)->flags)) != 0){
    return;
  }
  
  ags_panel_input_line_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_panel_input_line_disconnect(AgsConnectable *connectable)
{
  AgsPanelInputLine *panel_input_line;

  panel_input_line = AGS_PANEL_INPUT_LINE(connectable);

  if((AGS_LINE_CONNECTED & (AGS_LINE(panel_input_line)->flags)) == 0){
    return;
  }

  ags_panel_input_line_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_panel_input_line_finalize(GObject *gobject)
{
  /* empty */

  G_OBJECT_CLASS(ags_panel_input_line_parent_class)->finalize(gobject);
}

gchar*
ags_panel_input_line_get_name(AgsPlugin *plugin)
{
  return(AGS_PANEL_INPUT_LINE(plugin)->name);
}

void
ags_panel_input_line_set_name(AgsPlugin *plugin, gchar *name)
{
  AGS_PANEL_INPUT_LINE(plugin)->name = name;
}

gchar*
ags_panel_input_line_get_xml_type(AgsPlugin *plugin)
{
  return(AGS_PANEL_INPUT_LINE(plugin)->xml_type);
}

void
ags_panel_input_line_set_xml_type(AgsPlugin *plugin, gchar *xml_type)
{
  AGS_PANEL_INPUT_LINE(plugin)->xml_type = xml_type;
}

void
ags_panel_input_line_show(GtkWidget *line)
{
  GTK_WIDGET_CLASS(ags_panel_input_line_parent_class)->show(line);

  //  gtk_widget_hide(GTK_WIDGET(AGS_LINE(line)->group));
}

void
ags_panel_input_line_show_all(GtkWidget *line)
{
  GTK_WIDGET_CLASS(ags_panel_input_line_parent_class)->show_all(line);

  //  gtk_widget_hide(GTK_WIDGET(AGS_LINE(line)->group));
}

void
ags_panel_input_line_set_channel(AgsLine *line, AgsChannel *channel)
{
  AgsPanelInputLine *panel_input_line;

  AgsAudioConnection *audio_connection;
  
  GList *list;
  
  gchar *str;
  
  AGS_LINE_CLASS(ags_panel_input_line_parent_class)->set_channel(line, channel);

  panel_input_line = AGS_PANEL_INPUT_LINE(line);

  /*  */
  list = AGS_AUDIO(channel->audio)->audio_connection;
	  
  while((list = ags_audio_connection_find(list,
					  AGS_TYPE_INPUT,
					  channel->pad,
					  channel->audio_channel)) != NULL){
    GObject *data_object;

    g_object_get(G_OBJECT(list->data),
		 "data-object", &data_object,
		 NULL);
	    
    if(AGS_IS_SOUNDCARD(data_object)){
      break;
    }

    list = list->next;
  }

  if(list != NULL){
    audio_connection = list->data;
    
    str = g_strdup_printf("%s:%s[%d]",
			  G_OBJECT_TYPE_NAME(channel->soundcard),
			  ags_soundcard_get_device(AGS_SOUNDCARD(channel->soundcard)),
			  audio_connection->mapped_line);
    gtk_label_set_label(panel_input_line->soundcard_connection,
			str);

    g_signal_connect(audio_connection, "notify::data-object",
		     G_CALLBACK(ags_panel_input_line_notify_data_object_callback), panel_input_line);

    g_signal_connect(audio_connection, "notify::mapped-line",
		     G_CALLBACK(ags_panel_input_line_notify_mapped_line_callback), panel_input_line);
    
    g_free(str);
  }
  
#ifdef AGS_DEBUG
  g_message("ags_panel_input_line_set_channel - channel: %u",
	    channel->line);
#endif
}

void
ags_panel_input_line_map_recall(AgsLine *line,
				guint output_pad_start)
{
  AgsAudio *audio;
  AgsChannel *source;
  AgsChannel *current;

  AgsPlayChannel *play_channel;
  AgsPlayChannelRunMaster *play_channel_run;

  GList *list;

  if((AGS_LINE_MAPPED_RECALL & (line->flags)) != 0 ||
     (AGS_LINE_PREMAPPED_RECALL & (line->flags)) != 0){
    return;
  }
  
  audio = AGS_AUDIO(line->channel->audio);

  source = line->channel;

  /* ags-play */
  ags_recall_factory_create(audio,
			    NULL, NULL,
			    "ags-play-master",
			    source->audio_channel, source->audio_channel + 1,
			    source->pad, source->pad + 1,
			    (AGS_RECALL_FACTORY_INPUT,
			     AGS_RECALL_FACTORY_PLAY |
			     AGS_RECALL_FACTORY_ADD),
			    0);

  /* set audio channel */
  list = source->play;

  while((list = ags_recall_template_find_type(list,
					      AGS_TYPE_PLAY_CHANNEL)) != NULL){
    GValue audio_channel_value = {0,};

    play_channel = AGS_PLAY_CHANNEL(list->data);

    g_value_init(&audio_channel_value, G_TYPE_UINT64);
    g_value_set_uint64(&audio_channel_value,
		       source->audio_channel);
    ags_port_safe_write(play_channel->audio_channel,
			&audio_channel_value);
    g_value_unset(&audio_channel_value);

    list = list->next;
  }

  /* call parent */
  AGS_LINE_CLASS(ags_panel_input_line_parent_class)->map_recall(line,
								output_pad_start);
}

/**
 * ags_panel_input_line_new:
 * @channel: the assigned channel
 *
 * Creates an #AgsPanelInputLine
 *
 * Returns: a new #AgsPanelInputLine
 *
 * Since: 1.0.0
 */
AgsPanelInputLine*
ags_panel_input_line_new(AgsChannel *channel)
{
  AgsPanelInputLine *panel_input_line;

  panel_input_line = (AgsPanelInputLine *) g_object_new(AGS_TYPE_PANEL_INPUT_LINE,
							"channel", channel,
							NULL);

  return(panel_input_line);
}
