/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

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

GHashTable *ags_panel_input_line_message_monitor = NULL;

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

  if(ags_panel_input_line_message_monitor == NULL){
    ags_panel_input_line_message_monitor = g_hash_table_new_full(g_direct_hash, g_direct_equal,
								 NULL,
								 NULL);
  }

  g_hash_table_insert(ags_panel_input_line_message_monitor,
		      panel_input_line, ags_panel_input_line_message_monitor_timeout);
  
  g_timeout_add(1000 / 30, (GSourceFunc) ags_line_message_monitor_timeout, (gpointer) panel_input_line);

  /* mute line member */
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

  AgsAudio *audio;
  AgsAudioConnection *audio_connection;
  
  AgsMutexManager *mutex_manager;

  GObject *soundcard;
  
  GList *list;

  gchar *device;
  gchar *str;

  guint pad, audio_channel;
  
  pthread_mutex_t *application_mutex;
  pthread_mutex_t *soundcard_mutex;
  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *channel_mutex;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  panel_input_line = AGS_PANEL_INPUT_LINE(line);

  /* get channel mutex */
  pthread_mutex_lock(application_mutex);

  channel_mutex = ags_mutex_manager_lookup(mutex_manager,
					 (GObject *) channel);
  
  pthread_mutex_unlock(application_mutex);

  /* call parent */
  AGS_LINE_CLASS(ags_panel_input_line_parent_class)->set_channel(line,
								 channel);

  /* get some fields */
  pthread_mutex_lock(channel_mutex);

  soundcard = channel->soundcard;
  
  audio = channel->audio;

  pad = channel->pad;
  audio_channel = channel->audio_channel;
  
  pthread_mutex_unlock(channel_mutex);
  
  /* get audio mutex */
  pthread_mutex_lock(application_mutex);

  audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					 (GObject *) audio);
  
  pthread_mutex_unlock(application_mutex);

  /* find audio connection - soundcard */
  pthread_mutex_lock(audio_mutex);

  audio_connection = NULL;
  list = audio->audio_connection;
	  
  while((list = ags_audio_connection_find(list,
					  AGS_TYPE_INPUT,
					  pad,
					  audio_channel)) != NULL){
    GObject *data_object;

    g_object_get(G_OBJECT(list->data),
		 "data-object", &data_object,
		 NULL);
	    
    if(AGS_IS_SOUNDCARD(data_object)){
      audio_connection = list->data;
      
      break;
    }

    list = list->next;
  }

  pthread_mutex_unlock(audio_mutex);

  /* update label */
  if(audio_connection != NULL){
    /* get soundcard mutex */
    pthread_mutex_lock(application_mutex);

    soundcard_mutex = ags_mutex_manager_lookup(mutex_manager,
					     (GObject *) soundcard);
  
    pthread_mutex_unlock(application_mutex);

    /* get some fields */
    pthread_mutex_lock(soundcard_mutex);
    
    device = ags_soundcard_get_device(AGS_SOUNDCARD(soundcard));

    pthread_mutex_unlock(soundcard_mutex);

    /* label */
    str = g_strdup_printf("%s:%s[%d]",
			  G_OBJECT_TYPE_NAME(soundcard),
			  device,
			  audio_connection->mapped_line);
    gtk_label_set_label(panel_input_line->soundcard_connection,
			str);
    
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

  AgsPlayChannel *play_channel;
  AgsPlayChannelRunMaster *play_channel_run;
  
  AgsMutexManager *mutex_manager;

  GList *list;

  guint pad, audio_channel;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *source_mutex;

  if((AGS_LINE_MAPPED_RECALL & (line->flags)) != 0 ||
     (AGS_LINE_PREMAPPED_RECALL & (line->flags)) != 0){
    return;
  }
  
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  source = line->channel;

  /* get source mutex */
  pthread_mutex_lock(application_mutex);

  source_mutex = ags_mutex_manager_lookup(mutex_manager,
					  (GObject *) source);
  
  pthread_mutex_unlock(application_mutex);  

  /* get some fields */
  pthread_mutex_lock(source_mutex);

  audio = (AgsAudio *) source->audio;

  pad = source->pad;
  audio_channel = source->audio_channel;
  
  pthread_mutex_unlock(source_mutex);

  /* ags-play */
  ags_recall_factory_create(audio,
			    NULL, NULL,
			    "ags-play-master",
			    audio_channel, audio_channel + 1,
			    pad, pad + 1,
			    (AGS_RECALL_FACTORY_INPUT,
			     AGS_RECALL_FACTORY_PLAY |
			     AGS_RECALL_FACTORY_ADD),
			    0);

  /* set audio channel */
  pthread_mutex_lock(source_mutex);

  list = source->play;

  while((list = ags_recall_template_find_type(list,
					      AGS_TYPE_PLAY_CHANNEL)) != NULL){
    GValue audio_channel_value = {0,};

    play_channel = AGS_PLAY_CHANNEL(list->data);

    g_value_init(&audio_channel_value, G_TYPE_UINT64);
    g_value_set_uint64(&audio_channel_value,
		       audio_channel);
    ags_port_safe_write(play_channel->audio_channel,
			&audio_channel_value);
    g_value_unset(&audio_channel_value);

    list = list->next;
  }

  pthread_mutex_unlock(source_mutex);

  /* call parent */
  AGS_LINE_CLASS(ags_panel_input_line_parent_class)->map_recall(line,
								output_pad_start);
}

/**
 * ags_panel_input_line_message_monitor_timeout:
 * @panel_input_line: the #AgsPanelInputLine
 *
 * Monitor messages.
 *
 * Returns: %TRUE if proceed with redraw, otherwise %FALSE
 *
 * Since: 1.2.2
 */
gboolean
ags_panel_input_line_message_monitor_timeout(AgsPanelInputLine *panel_input_line)
{
  if(g_hash_table_lookup(ags_panel_input_line_message_monitor,
			 panel_input_line) != NULL){
    AgsAudio *audio;
    AgsChannel *channel;
    AgsConnection *connection;

    AgsMutexManager *mutex_manager;
    AgsMessageDelivery *message_delivery;

    GList *message_start, *message;
    GList *list;

    guint pad, audio_channel;
    
    pthread_mutex_t *application_mutex;
    pthread_mutex_t *soundcard_mutex;
    pthread_mutex_t *audio_mutex;
    pthread_mutex_t *channel_mutex;
    
    /* get mutex manager and application mutex */
    mutex_manager = ags_mutex_manager_get_instance();
    application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

    /* message delivery */
    message_delivery = ags_message_delivery_get_instance();

    channel = AGS_LINE(panel_input_line)->channel;

    /* get channel mutex */
    pthread_mutex_lock(application_mutex);

    channel_mutex = ags_mutex_manager_lookup(mutex_manager,
					     (GObject *) channel);

    pthread_mutex_unlock(application_mutex);

    /* get some fields */
    pthread_mutex_lock(channel_mutex);
    
    audio = channel->audio;

    pad = channel->pad;
    audio_channel = channel->audio_channel;
    
    pthread_mutex_unlock(channel_mutex);

    /* get audio mutex */
    pthread_mutex_lock(application_mutex);

    audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					     (GObject *) audio);

    pthread_mutex_unlock(application_mutex);
    
    /* libags - retrieve message */
    pthread_mutex_lock(audio_mutex);

    list = audio->audio_connection;
    connection = NULL;
    
    while((list = ags_audio_connection_find(list,
					    AGS_TYPE_INPUT,
					    pad,
					    audio_channel)) != NULL){
      GObject *data_object;

      g_object_get(G_OBJECT(list->data),
		   "data-object", &data_object,
		   NULL);
	    
      if(AGS_IS_SOUNDCARD(data_object)){
	connection = list->data;
	
	break;
      }

      list = list->next;
    }

    pthread_mutex_unlock(audio_mutex);

    /* check messages */
    message_start = 
	message = ags_message_delivery_find_sender(message_delivery,
						   "libags",
						   connection);

    while(message != NULL){
      xmlNode *root_node;

      root_node = xmlDocGetRootElement(AGS_MESSAGE_ENVELOPE(message->data)->doc);
      
      if(!xmlStrncmp(root_node->name,
		     "ags-command",
		     12)){
	if(!xmlStrncmp(xmlGetProp(root_node,
				  "method"),
		       "GObject::notify::data-object",
		       28)){
	  GObject *soundcard;

	  gchar *device;
	  gchar *str;

	  guint mapped_line;
	  
	  GValue *value;

	  /* get some fields */
	  pthread_mutex_lock(audio_mutex);

	  mapped_line = AGS_AUDIO_CONNECTION(connection)->mapped_line;
	  
	  pthread_mutex_unlock(audio_mutex);

	  /* get data object */
	  value = ags_parameter_find(AGS_MESSAGE_ENVELOPE(message->data)->parameter, AGS_MESSAGE_ENVELOPE(message->data)->n_params,
				     "data-object");
	  soundcard = g_value_get_object(value);

	  /* get soundcard mutex */
	  pthread_mutex_lock(application_mutex);

	  soundcard_mutex = ags_mutex_manager_lookup(mutex_manager,
						     (GObject *) soundcard);

	  pthread_mutex_unlock(application_mutex);

	  /* get some fields */
	  pthread_mutex_lock(soundcard_mutex);
	  
	  device = ags_soundcard_get_device(AGS_SOUNDCARD(soundcard));

	  pthread_mutex_unlock(soundcard_mutex);
	  
	  /* update label */
	  str = g_strdup_printf("%s:%s[%d]",
				G_OBJECT_TYPE_NAME(soundcard),
				device,
				mapped_line + 1);
	  gtk_label_set_label(panel_input_line->soundcard_connection,
			      str);

	  g_free(str);
	}
      }
      
      ags_message_delivery_remove_message(message_delivery,
					  "libags",
					  message->data);
      
      message = message->next;
    }
  
    g_list_free_full(message_start,
		     ags_message_envelope_free);

    /* libags-audio - retrieve message */
    //NOTE:JK: very same connection object
    
    /* check messages */
    message_start = 
	message = ags_message_delivery_find_sender(message_delivery,
						   "libags-audio",
						   connection);

    while(message != NULL){
      xmlNode *root_node;

      root_node = xmlDocGetRootElement(AGS_MESSAGE_ENVELOPE(message->data)->doc);
      
      if(!xmlStrncmp(root_node->name,
		     "ags-command",
		     12)){
	if(!xmlStrncmp(xmlGetProp(root_node,
				  "method"),
		       "GObject::notify::mapped-line",
		       28)){
	  GObject *soundcard;

	  gchar *device;
	  gchar *str;

	  guint mapped_line;
	  
	  GValue *value;

	  /* get some fields */
	  pthread_mutex_lock(audio_mutex);

	  soundcard = connection->data_object;
	  
	  pthread_mutex_unlock(audio_mutex);

	  /* get mapped line */
	  value = ags_parameter_find(AGS_MESSAGE_ENVELOPE(message->data)->parameter, AGS_MESSAGE_ENVELOPE(message->data)->n_params,
				     "mapped-line");
	  mapped_line = g_value_get_uint(value);

	  /* get soundcard mutex */
	  pthread_mutex_lock(application_mutex);

	  soundcard_mutex = ags_mutex_manager_lookup(mutex_manager,
						     (GObject *) soundcard);

	  pthread_mutex_unlock(application_mutex);

	  /* get some fields */
	  pthread_mutex_lock(soundcard_mutex);
	  
	  device = ags_soundcard_get_device(AGS_SOUNDCARD(soundcard));

	  pthread_mutex_unlock(soundcard_mutex);
	  
	  /* update label */
	  str = g_strdup_printf("%s:%s[%d]",
				G_OBJECT_TYPE_NAME(soundcard),
				device,
				mapped_line + 1);
	  gtk_label_set_label(panel_input_line->soundcard_connection,
			      str);

	  g_free(str);
	}
      }
      
      ags_message_delivery_remove_message(message_delivery,
					  "libags-audio",
					  message->data);
      
      message = message->next;
    }
  
    g_list_free_full(message_start,
		     ags_message_envelope_free);

    return(TRUE);
  }else{
    return(FALSE);
  }
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
